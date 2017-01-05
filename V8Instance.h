#ifndef _V8_INSTANCE_H_
#define _V8_INSTANCE_H_

#include <node.h>

struct V8Instance {
    v8::Isolate *isolate;
    bool available;

    V8Instance();
};

#endif