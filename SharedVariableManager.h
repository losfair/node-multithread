#ifndef _GLOBAL_VARIABLE_MANAGER_H_
#define _GLOBAL_VARIABLE_MANAGER_H_

#include <string>

namespace SharedVariableManager {
    std::string getVariableValue(const std::string& name);
    void setVariableValue(const std::string& name, const std::string& value);
}

#endif
