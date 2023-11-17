//
// Created by TaxMachine on 2023-11-08.
//

#include "injection.hpp"

#include <any>
#include <regex>

#include "exceptions.hpp"
#include "utils.hpp"

#ifdef WIN32
    #include <windows.h>
    #include <psapi.h>
#elif __linux__
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include "injector.h"
#endif

#ifdef __linux
injector_t* injector = nullptr;
void* handle;
#endif

#ifdef WIN32
static HANDLE getProcessHandleFromWindowName(const std::string& windowName) {
    const HWND& window = FindWindowA(nullptr, windowName.c_str());
    if (!window) {
        throw InjectionException("Window not found");
    }
    DWORD processId;
    GetWindowThreadProcessId(window, &processId) ?
        throw InjectionException("GetWindowThreadProcessId Failed") : 0;

    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}
#endif

void Injection::inject(const std::string& dllPath, const unsigned long& pid) {
#ifdef WIN32
    const HANDLE& processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (processHandle == nullptr)
        throw InjectionException("Process not found");

    const LPVOID& dllPathAddress = VirtualAllocEx(
        processHandle,
        nullptr,
        dllPath.size() + 1,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE);

    if (dllPathAddress == nullptr)
        throw InjectionException("VirtualAllocEx Failed");

    WriteProcessMemory(
        processHandle,
        dllPathAddress,
        dllPath.c_str(),
        dllPath.size() + 1,
        nullptr) == FALSE ?
            throw InjectionException("WriteProcessMemory Failed") : 0;

    const auto& loadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (loadLibraryAddress == nullptr)
        throw InjectionException("GetProcAddress Failed");

    const HANDLE& thread = CreateRemoteThread(
        processHandle,
        nullptr,
        0,
        std::any_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddress),
        dllPathAddress,
        0,
        nullptr);

    if (thread == nullptr)
        throw InjectionException("CreateRemoteThread Failed");

    WaitForSingleObject(thread,INFINITE) == WAIT_FAILED ?
        throw InjectionException("WaitForSingleObject Failed") : 0;

    CloseHandle(processHandle);
#elif __linux__
    if (injector != nullptr)
        throw InjectionException("Already injected");;
    injector_attach(&injector, (int)pid);
    if (injector == nullptr)
        throw InjectionException("Attach failed");
    if (injector_inject(injector, dllPath.c_str(), &handle)) {
        injector_detach(injector);
        throw InjectionException("Injection failed");
    }
#endif
}

#ifdef __linux
void Injection::uninject() {
    if (injector == nullptr)
        throw InjectionException("Not injected");;
    injector_uninject(injector, handle);
    injector_detach(injector);
    injector = nullptr;
}
#endif

std::vector<MCInstance> Injection::getMinecraftVersions() {
#ifdef WIN32
    std::vector<MCInstance> versions;

    HWND window = FindWindowA(nullptr, nullptr);
    if (!window)
        throw InjectionException("Window not found");
    while (window) {
        char title[256];
        GetWindowTextA(window, title, 256);
        if (std::string{title}.find("Minecraft ") != std::string::npos) {
            std::string titleString{title};
            std::regex regex("Minecraft ([0-9]+\\.[0-9]+\\.[0-9]+)");
            std::smatch match;
            std::regex_search(titleString, match, regex);

            if (std::stoi(Utils::split(match[1].str(), ".")[1]) <= 12)
                continue;

            MCInstance instance;
            instance.title = titleString;
            DWORD processId;
            !GetWindowThreadProcessId(window, &processId) ?
                throw InjectionException("GetWindowThreadProcessId Failed") : 0;
            instance.pid = processId;
            versions.push_back(instance);
        }
        window = GetWindow(window, GW_HWNDNEXT);
    }

    return versions;
#elif __linux__
    std::vector<MCInstance> instances;
    FILE* fp = popen("ps -e | grep \"\\d+(?= [^: ].+?java)\" -Po", "r");

    size_t length;
    char* line = nullptr;
    while (getline(&line, &length, fp) != -1) {
        char* newline = strchr(line, '\n');
        if (newline != nullptr)
            *newline = 0;

        char path[32];
        snprintf(path, 32, "/proc/%s/cmdline", line);

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            continue;
        char buffer[256];
        std::string cmdline;
        while (file.read(buffer, sizeof(buffer)))
            cmdline += buffer;
        file.close();
        if (cmdline.empty())
            continue;

        std::regex regex("/.minecraft/versions/([0-9]+\\.[0-9]+\\.[0-9]+)/");
        std::smatch match;
        if (!std::regex_search(cmdline, match, regex))
            continue;
        std::string version = match[1].str();
        if (std::stoi(Utils::split(version, ".")[1]) <= 12)
            continue;

        instances.push_back({version, (unsigned long)(atol(line))});
    }
    pclose(fp);
    free(line);
    return instances;
#endif
}