//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_INJECTION_HPP
#define FRIDGE_INJECTOR_INJECTION_HPP

#include <string>
#include <vector>

struct MCInstance {
    std::string title;
    int pid;
    std::string javawPath;
};

namespace Injection {
    void inject(const std::string& dllPath, const std::string& windowName);
    std::vector<MCInstance> getMinecraftVersions();
}

#endif //FRIDGE_INJECTOR_INJECTION_HPP
