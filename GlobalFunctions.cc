#include <node.h>

#include <stdio.h>

using namespace v8;

namespace GlobalFunctions {
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

    void init(Isolate *isolate, Local<ObjectTemplate>& globalObject) {
        globalObject -> Set(
            String::NewFromUtf8(isolate, "_API_consoleLog"),
            FunctionTemplate::New(isolate, consoleLog)
        );
    }
}