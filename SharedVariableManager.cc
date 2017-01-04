#include <string>
#include <mutex>
#include <map>

namespace SharedVariableManager {
    static std::map<std::string, std::string> variables;
    static std::mutex variablesLock;

    std::string getVariableValue(const std::string& name) {
        variablesLock.lock();

        auto itr = variables.find(name);

        if(itr == variables.end()) {
            variablesLock.unlock();
            return (std::string) "";
        }

        std::string value = itr -> second;

        variablesLock.unlock();
        return value;
    }

    void setVariableValue(const std::string& name, const std::string& value) {
        variablesLock.lock();
        variables[name] = value;
        variablesLock.unlock();
    }
}
