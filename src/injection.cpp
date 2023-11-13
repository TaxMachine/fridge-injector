//
// Created by TaxMachine on 2023-11-08.
//

#include "injection.hpp"

#include <regex>

#include "exceptions.hpp"
#include "utils.hpp"

#ifdef WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
#endif

#ifdef WIN32
static HANDLE getProcessHandleFromWindowName(const std::string& windowName) {
    HWND window = FindWindowA(nullptr, windowName.c_str());
    if (!window) {
        throw InjectionException("Window not found");
    }
    DWORD processId;
    if (GetWindowThreadProcessId(window, &processId) == 0) {
        throw InjectionException("GetWindowThreadProcessId Failed");
    }
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}
#endif

void Injection::inject(const std::string& dllPath, const std::string& windowName) {
#ifdef WIN32
    HANDLE processHandle = getProcessHandleFromWindowName(windowName);
    if (!processHandle)
        throw InjectionException("Process not found");

    LPVOID dllPathAddress = VirtualAllocEx(processHandle,nullptr,dllPath.size() + 1,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
    if (!dllPathAddress)
        throw InjectionException("VirtualAllocEx Failed");

    if (WriteProcessMemory(processHandle,dllPathAddress,dllPath.c_str(),dllPath.size() + 1,nullptr))
        throw InjectionException("WriteProcessMemory Failed");

    HANDLE thread = CreateRemoteThread(processHandle,nullptr,0,(LPTHREAD_START_ROUTINE) LoadLibraryA,dllPathAddress,0,nullptr);
    if (!thread)
        throw InjectionException("CreateRemoteThread Failed");

    if (WaitForSingleObject(thread,INFINITE))
        throw InjectionException("WaitForSingleObject Failed");

    CloseHandle(thread);
    if (VirtualFreeEx(processHandle,dllPathAddress,dllPath.size() + 1,MEM_RELEASE))
        throw InjectionException("VirtualFreeEx Failed");

    CloseHandle(processHandle);
#elif __linux__
    throw NotImplementedException("Linux is not supported");
#endif
}

std::vector<std::string> Injection::getMinecraftVersions() {
#ifdef WIN32
    std::vector<std::string> versions;

    HWND window = FindWindowA(nullptr, nullptr);
    while (window) {
        char title[256];
        GetWindowTextA(window, title, 256);
        std::string titleString = title;
        if (titleString.find("Minecraft ") != std::string::npos) {
            std::regex regex("Minecraft ([0-9]+\\.[0-9]+\\.[0-9]+)");
            std::smatch match;
            std::regex_search(titleString, match, regex);
            std::string version = match[1];

            std::vector<std::string> splitVersion = split(version, ".");
            if (std::stoi(splitVersion[1]) <= 12)
                continue;

            versions.push_back(version);
        }
        window = GetWindow(window, GW_HWNDNEXT);
    }

    return versions;
#elif __linux__
    throw NotImplementedException("Linux is not supported");
#endif
}