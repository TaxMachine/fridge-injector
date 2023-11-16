//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_INJECTION_HPP
#define FRIDGE_INJECTOR_INJECTION_HPP

#include <string>
#include <vector>

struct MCInstance {
    std::string title;
    unsigned long pid;
};

namespace Injection {
    void inject(const std::string& dllPath, const unsigned long& pid);
    std::vector<MCInstance> getMinecraftVersions();
}

#endif //FRIDGE_INJECTOR_INJECTION_HPP
