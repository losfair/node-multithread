#ifndef _GLOBAL_FUNCTIONS_H_
#define _GLOBAL_FUNCTIONS_H_

#include <node.h>

namespace GlobalFunctions {
    void init(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate>& globalObject);
}

#endif
