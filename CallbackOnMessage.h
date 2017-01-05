#ifndef _CALLBACK_ON_MESSAGE_H_
#define _CALLBACK_ON_MESSAGE_H_

#include <string>
#include <functional>

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

        CallbackArgs();
        CallbackArgs(const std::string& _sourceThreadName, std::function<void(const CallbackArgs *)> _target, std::string& _data, MessageType _type);
        CallbackArgs(const std::string& _sourceThreadName, std::function<void(const CallbackArgs *)> _target, MessageType _type);
    };
}

#endif
