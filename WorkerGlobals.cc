#include <node.h>

#include "SharedFunctions.h"

#include <stdio.h>

using namespace v8;

namespace WorkerGlobals {
    static void consoleLog(const FunctionCallbackInfo<Value>& info) {
        if(
            info.Length() != 1
            || !info[0] -> IsString()
        ) {
            return;
        }

        String::Utf8Value _msg(info[0] -> ToString());
        printf("%s\n", *_msg);
    }

    void init(Isolate *isolate, Local<ObjectTemplate>& globalObject, const std::string& threadName) {
        globalObject -> Set(
            String::NewFromUtf8(isolate, "__THREAD_NAME__"),
            String::NewFromUtf8(isolate, threadName.c_str())
        );
        globalObject -> Set(
            String::NewFromUtf8(isolate, "_API_consoleLog"),
            FunctionTemplate::New(isolate, consoleLog)
        );
        globalObject -> Set(
            String::NewFromUtf8(isolate, "_API_getSharedVariableValue"),
            FunctionTemplate::New(isolate, SharedFunctions::onGetSharedVariableValue)
        );
        globalObject -> Set(
            String::NewFromUtf8(isolate, "_API_setSharedVariableValue"),
            FunctionTemplate::New(isolate, SharedFunctions::onSetSharedVariableValue)
        );
    }
}
