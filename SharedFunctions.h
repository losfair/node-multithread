#ifndef _SHARED_FUNCTIONS_H_
#define _SHARED_FUNCTIONS_H_

#include <node.h>

namespace SharedFunctions {
    void onGetSharedVariableValue(const v8::FunctionCallbackInfo<v8::Value>& info);
    void onSetSharedVariableValue(const v8::FunctionCallbackInfo<v8::Value>& info);
}


#endif
