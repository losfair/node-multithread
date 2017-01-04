#include <node.h>

#include <string>
#include <map>
#include <functional>

#include <stdio.h>

#include "ThreadCore.h"
#include "GlobalFunctions.h"
#include "SharedFunctions.h"

using namespace v8;

static bool loadScript(Isolate *isolate, Local<Context>& cxt, const std::string& targetCode) {
    Local<String> _targetCode = String::NewFromUtf8(isolate, targetCode.c_str(), NewStringType::kNormal).ToLocalChecked();

    TryCatch exceptionCatcher(isolate);

    MaybeLocal<Script> _compiledScript = Script::Compile(cxt, _targetCode);

    if(_compiledScript.IsEmpty()) {
        printf("Unable to compile script.\n");
        return false;
    }

    Local<Script> compiledScript = _compiledScript.ToLocalChecked();

    compiledScript -> Run(cxt);

    if(exceptionCatcher.HasCaught()) {
        printf("Exception caught during script execution.\n");
        return false;
    }

    return true;
}

static std::function<void()> newV8Thread(std::string& threadName, std::string& targetCode) {
    return [threadName, targetCode]() {
        Isolate::CreateParams isolateParams;
        isolateParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();

        Isolate *newIsolate = Isolate::New(isolateParams);

        {
            Locker threadLocker(newIsolate);

            Isolate::Scope isolateScope(newIsolate);
            HandleScope handleScope(newIsolate);

            Local<ObjectTemplate> globalObject = ObjectTemplate::New(newIsolate);
            GlobalFunctions::init(newIsolate, globalObject);

            Local<Context> newContext = Context::New(newIsolate, NULL, globalObject);
            Context::Scope contextScope(newContext);

            loadScript(newIsolate, newContext, targetCode);
        }

        newIsolate -> Dispose();
    };
}

static void onCreateThread(const FunctionCallbackInfo<Value>& info) {
    Isolate *isolate = info.GetIsolate();

    if(
        info.Length() != 2
        || !info[0] -> IsString() // Thread name
        || !info[1] -> IsString() // Script code
    ) {
        isolate -> ThrowException(String::NewFromUtf8(isolate, "Invalid parameters"));
        return;
    }

    Local<String> _threadName = info[0] -> ToString();
    std::string threadName = *String::Utf8Value(_threadName);

    Local<String> _targetCode = info[1] -> ToString();
    std::string targetCode = *String::Utf8Value(_targetCode);

    ThreadCore::createThread(threadName, newV8Thread(threadName, targetCode));
}

static void moduleInit(Local<Object> exports) {
    NODE_SET_METHOD(exports, "createThread", onCreateThread);
    NODE_SET_METHOD(exports, "getSharedVariableValue", SharedFunctions::onGetSharedVariableValue);
    NODE_SET_METHOD(exports, "setSharedVariableValue", SharedFunctions::onSetSharedVariableValue);
}

NODE_MODULE(MultithreadCore, moduleInit)
