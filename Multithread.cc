#include <node.h>
#include <uv.h>

#include <string>
#include <map>
#include <functional>
#include <atomic>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>

#include "ThreadCore.h"
#include "WorkerGlobals.h"
#include "SharedFunctions.h"
#include "CallbackOnMessage.h"
#include "V8Instance.h"

using namespace v8;

static std::string loadScript(Isolate *isolate, Local<Context>& cxt, const std::string& targetCode) {
    Local<String> _targetCode = String::NewFromUtf8(isolate, targetCode.c_str(), NewStringType::kNormal).ToLocalChecked();

    TryCatch exceptionCatcher(isolate);

    MaybeLocal<Script> _compiledScript = Script::Compile(cxt, _targetCode);

    if(_compiledScript.IsEmpty()) {
        printf("Unable to compile script.\n");
        return std::string();
    }

    Local<Script> compiledScript = _compiledScript.ToLocalChecked();

    MaybeLocal<Value> maybeResult = compiledScript -> Run(cxt);

    if(exceptionCatcher.HasCaught()) {
        printf("Exception caught during script execution.\n");
        return std::string();
    }

    if(maybeResult.IsEmpty()) {
        return std::string();
    }

    Local<Value> result = maybeResult.ToLocalChecked();

    return std::string(*String::Utf8Value(result -> ToString()));
}

std::atomic<int> totalThreadCount(0);
const int maxTotalThreadCount = 8;

V8Instance *v8Instances = NULL;

V8Instance * getNextAvailableV8Instance() {
    for(int i = 0; i < maxTotalThreadCount; i++) {
        if(v8Instances[i].available) {
            return &v8Instances[i];
        }
    }
    return NULL;
}

static void initV8Instances() {
    v8Instances = new V8Instance[maxTotalThreadCount];
    for(int i = 0; i < maxTotalThreadCount; i++) {
        Isolate::CreateParams isolateParams;
        isolateParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
        v8Instances[i].isolate = Isolate::New(isolateParams);
    }
}

static void uvCallbackFromThread(uv_async_t *handle) {
    if(!handle -> data) return;

    CallbackOnMessage::CallbackArgs *args = (CallbackOnMessage::CallbackArgs *) handle -> data;
    handle -> data = NULL;

    std::function<void(const CallbackOnMessage::CallbackArgs *)>& cb = args -> target;

    if(args -> type == CallbackOnMessage::MESSAGE_TYPE_DONE) {
        uv_close((uv_handle_t *) handle, (uv_close_cb) free);
        ThreadCore::joinThread(args -> sourceThreadName);
        ThreadCore::removeThread(args -> sourceThreadName);
        totalThreadCount--;

        if(args -> v8Instance) {
            args -> v8Instance -> available = true;
        }
    }

    cb(args);

    delete args;
}

static std::function<void()> newV8Thread(uv_async_t *asyncContext, std::string& threadName, std::string& targetCode, std::function<void(const CallbackOnMessage::CallbackArgs *)> callbackOnMessage) {
    return [threadName, targetCode, asyncContext, callbackOnMessage]() {
        V8Instance *targetInstance = getNextAvailableV8Instance();

        if(!targetInstance) {
            std::string errMsg("No V8 instance available");
            asyncContext -> data = (void *) new CallbackOnMessage::CallbackArgs(
                threadName,
                NULL,
                callbackOnMessage,
                errMsg,
                CallbackOnMessage::MESSAGE_TYPE_DONE
            );
            uv_async_send(asyncContext);
            return;
        }

        targetInstance -> available = false;

        Isolate *newIsolate = targetInstance -> isolate;

        std::string result;

        {
            Locker threadLocker(newIsolate);

            Isolate::Scope isolateScope(newIsolate);
            HandleScope handleScope(newIsolate);

            Local<ObjectTemplate> globalObject = ObjectTemplate::New(newIsolate);
            WorkerGlobals::init(newIsolate, globalObject, threadName);

            Local<Context> newContext = Context::New(newIsolate, NULL, globalObject);
            Context::Scope contextScope(newContext);

            result = loadScript(newIsolate, newContext, targetCode);
        }

        asyncContext -> data = (void *) new CallbackOnMessage::CallbackArgs(
            threadName,
            targetInstance,
            callbackOnMessage,
            result,
            CallbackOnMessage::MESSAGE_TYPE_DONE
        );
        uv_async_send(asyncContext);
    };
}

static void onCreateThread(const FunctionCallbackInfo<Value>& info) {
    Isolate *isolate = info.GetIsolate();

    if(
        info.Length() != 3
        || !info[0] -> IsString() // Thread name
        || !info[1] -> IsString() // Script code
        || !info[2] -> IsFunction() // Callback on done
    ) {
        isolate -> ThrowException(String::NewFromUtf8(isolate, "Invalid parameters"));
        return;
    }

    if(totalThreadCount >= maxTotalThreadCount) {
        isolate -> ThrowException(String::NewFromUtf8(isolate, "Too many threads"));
        return;
    }

    Local<String> _threadName = info[0] -> ToString();
    std::string threadName = *String::Utf8Value(_threadName);

    Local<String> _targetCode = info[1] -> ToString();
    std::string targetCode = *String::Utf8Value(_targetCode);

    Local<Function> _cb = Local<Function>::Cast(info[2]);
    Persistent<Function, CopyablePersistentTraits<Function>> cb(isolate, _cb);

    uv_async_t *asyncContext = (uv_async_t *) malloc(sizeof(uv_async_t));
    uv_loop_t *defaultLoop = uv_default_loop();

    asyncContext -> data = NULL;

    uv_async_init(defaultLoop, asyncContext, uvCallbackFromThread);

    bool createResult = ThreadCore::createThread(
        threadName,
        newV8Thread(asyncContext, threadName, targetCode, [cb](const CallbackOnMessage::CallbackArgs *msg) {
            Isolate *isolate = Isolate::GetCurrent();
            HandleScope currentScope(isolate);

            Local<Value> cbArgs[] = {
                String::NewFromUtf8(isolate, msg -> data.c_str()),
                Number::New(isolate, msg -> type)
            };

            Local<Function> cbLocal = Local<Function>::New(isolate, cb);
            cbLocal -> Call(Null(isolate), 2, cbArgs); // On message
            ((Persistent<Function, CopyablePersistentTraits<Function> >) cb).Reset();
        })
    );

    if(!createResult) {
        uv_close((uv_handle_t *) asyncContext, (uv_close_cb) free);
        isolate -> ThrowException(String::NewFromUtf8(isolate, "Thread creation failed"));
    }

    totalThreadCount++;
}

static void onGetMaxThreadCount(const FunctionCallbackInfo<Value>& info) {
    Isolate *isolate = info.GetIsolate();

    info.GetReturnValue().Set(Number::New(isolate, maxTotalThreadCount));
}

static void moduleInit(Local<Object> exports) {
    initV8Instances();

    NODE_SET_METHOD(exports, "createThread", onCreateThread);
    NODE_SET_METHOD(exports, "getMaxThreadCount", onGetMaxThreadCount);
    NODE_SET_METHOD(exports, "getSharedVariableValue", SharedFunctions::onGetSharedVariableValue);
    NODE_SET_METHOD(exports, "setSharedVariableValue", SharedFunctions::onSetSharedVariableValue);
}

NODE_MODULE(MultithreadCore, moduleInit)
