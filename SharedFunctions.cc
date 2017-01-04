#include <node.h>

#include "SharedVariableManager.h"

using namespace v8;

namespace SharedFunctions {
    void onGetSharedVariableValue(const FunctionCallbackInfo<Value>& info) {
        Isolate *isolate = info.GetIsolate();

        if(
            info.Length() != 1
            || !info[0] -> IsString() // Variable name
        ) {
            isolate -> ThrowException(String::NewFromUtf8(isolate, "Invalid parameters"));
            return;
        }

        Local<String> _variableName = info[0] -> ToString();
        std::string variableName = *String::Utf8Value(_variableName);

        std::string variableValue = SharedVariableManager::getVariableValue(variableName);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, variableValue.c_str()));
    }

    void onSetSharedVariableValue(const FunctionCallbackInfo<Value>& info) {
        Isolate *isolate = info.GetIsolate();

        if(
            info.Length() != 2
            || !info[0] -> IsString() // Variable name
            || !info[1] -> IsString() // Variable value
        ) {
            isolate -> ThrowException(String::NewFromUtf8(isolate, "Invalid parameters"));
            return;
        }

        Local<String> _variableName = info[0] -> ToString();
        std::string variableName = *String::Utf8Value(_variableName);

        Local<String> _variableValue = info[1] -> ToString();
        std::string variableValue = *String::Utf8Value(_variableValue);

        SharedVariableManager::setVariableValue(variableName, variableValue);
    }
}
