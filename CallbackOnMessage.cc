#include <string>
#include <functional>

#include "CallbackOnMessage.h"

namespace CallbackOnMessage {
    CallbackArgs::CallbackArgs() {
        type = MESSAGE_TYPE_NORMAL;
    }

    CallbackArgs::CallbackArgs(const std::string& _sourceThreadName, V8Instance *_v8Instance, std::function<void(const CallbackArgs *)> _target, std::string& _data, MessageType _type) {
        sourceThreadName = _sourceThreadName;
        v8Instance = _v8Instance;
        target = _target;
        data = _data;
        type = _type;
    }

    CallbackArgs::CallbackArgs(const std::string& _sourceThreadName, V8Instance *_v8Instance, std::function<void(const CallbackArgs *)> _target, MessageType _type) {
        sourceThreadName = _sourceThreadName;
        v8Instance = _v8Instance;
        target = _target;
        type = _type;
    }
}
