#ifndef _CALLBACK_ON_MESSAGE_H_
#define _CALLBACK_ON_MESSAGE_H_

#include <string>
#include <functional>

#include "V8Instance.h"

namespace CallbackOnMessage {
    enum MessageType {
        MESSAGE_TYPE_NORMAL,
        MESSAGE_TYPE_DONE
    };

    struct CallbackArgs {
        std::string sourceThreadName;
        std::function<void(const CallbackArgs *)> target;
        std::string data;
        MessageType type;
        V8Instance *v8Instance;

        CallbackArgs();
        CallbackArgs(const std::string& _sourceThreadName, V8Instance *_v8Instance, std::function<void(const CallbackArgs *)> _target, std::string& _data, MessageType _type);
        CallbackArgs(const std::string& _sourceThreadName, V8Instance *_v8Instance, std::function<void(const CallbackArgs *)> _target, MessageType _type);
    };
}

#endif
