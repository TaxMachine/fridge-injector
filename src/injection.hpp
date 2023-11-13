//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_INJECTION_HPP
#define FRIDGE_INJECTOR_INJECTION_HPP

#include <string>
#include <vector>

namespace Injection {
    void inject(const std::string& dllPath, const std::string& windowName);
    std::vector<std::string> getMinecraftVersions();
}

#endif //FRIDGE_INJECTOR_INJECTION_HPP
