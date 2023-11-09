//
// Created by TaxMachine on 2023-11-08.
//

#ifndef FRIDGE_INJECTOR_INJECTION_HPP
#define FRIDGE_INJECTOR_INJECTION_HPP

#include <string>
#include <windows.h>

namespace Injection {
    void inject(std::string dllPath, HANDLE processHandle);
}

#endif //FRIDGE_INJECTOR_INJECTION_HPP
