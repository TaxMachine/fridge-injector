//
// Created by taxis on 2023-11-13.
//

#ifndef FRIDGE_INJECTOR_UTILS_HPP
#define FRIDGE_INJECTOR_UTILS_HPP

#include <string>
#include <vector>
#include <windows.h>

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

static std::vector<std::string> split(std::string str, const std::string& delim) {
    std::vector<std::string> split;
    size_t pos;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        split.push_back(token);
        str.erase(0, pos + delim.length());
    }
    split.push_back(str);
    return split;
}

static std::string sha1(const std::string& path) {
#ifdef WIN32
    std::string command = "certutil.exe -hashfile " + path + " SHA1";
#elif __linux__
    std::string command = "sha256sum " + path;
#endif
    FILE* file = popen(command.c_str(), "r");
    if (!file)
        return "Failed to open file";
    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), file) != nullptr)
        output += buffer;
    pclose(file);
#ifdef WIN32
    return split(output, "\n")[1];
#elif __linux__
    return split(output, " ")[0];
#endif
}

static std::string findDllSymbol(const std::string& dllPath, const std::string& symbol) {
#ifdef WIN32
    HMODULE dll = LoadLibraryA(dllPath.c_str());
    if (!dll)
        return "Failed to load dll";
    FARPROC proc = GetProcAddress(dll, symbol.c_str());
    if (!proc)
        return "Failed to find symbol";
    char** (*getExportedSymbol)() = (char**(*)()) proc;
    return *getExportedSymbol();
#elif __linux__
    void *dll = dlopen(dllPath.c_str(), RTLD_LAZY);
    if (!dll)
        return "Failed to load shared library";
    void *proc = dlsym(dll, symbol.c_str());
    if (!proc)
        return "Failed to find symbol";
    char** (*getExportedSymbol)() = (char**(*)()) proc;
    dlclose(dll);
    return *getExportedSymbol();
#endif

}

#endif //FRIDGE_INJECTOR_UTILS_HPP
