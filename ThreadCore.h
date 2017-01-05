#ifndef _THREAD_CORE_H_
#define _THREAD_CORE_H_

#include <string>
#include <functional>

namespace ThreadCore {
    bool createThread(std::string threadName, std::function<void()> payload);
    bool joinThread(std::string threadName);
    bool removeThread(std::string threadName);
}

#endif
