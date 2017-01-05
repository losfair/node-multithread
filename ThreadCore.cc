#include <thread>
#include <map>
#include <string>
#include <mutex>
#include <functional>

namespace ThreadCore {
    static std::map<std::string, std::thread *> threads;
    static std::mutex thread_info_global_lock;

    bool createThread(std::string threadName, std::function<void()> payload) {
        thread_info_global_lock.lock();

        if(threads.find(threadName) != threads.end()) { // A thread with the same name is already running
            thread_info_global_lock.unlock();
            return false;
        }

        std::thread *newThread = new std::thread(payload);
        threads[threadName] = newThread;

        thread_info_global_lock.unlock();
        return true;
    }

    bool joinThread(std::string threadName) {
        thread_info_global_lock.lock();

        auto itr = threads.find(threadName);

        if(itr == threads.end()) { // A thread with the given name doesn't exist
            thread_info_global_lock.unlock();
            return false;
        }

        std::thread *targetThread = itr -> second;
        thread_info_global_lock.unlock();

        targetThread -> join();
        
        return true;
    }

    bool removeThread(std::string threadName) {
        thread_info_global_lock.lock();

        auto itr = threads.find(threadName);

        if(itr == threads.end()) { // A thread with the given name doesn't exist
            thread_info_global_lock.unlock();
            return false;
        }

        std::thread *targetThread = itr -> second;
        delete targetThread;

        threads.erase(itr);

        thread_info_global_lock.unlock();
        return true;
    }
}
