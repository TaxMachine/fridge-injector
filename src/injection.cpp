//
// Created by TaxMachine on 2023-11-08.
//

#include "injection.hpp"

#include <stdexcept>
#include <windows.h>
#include <regex>


static HANDLE getProcessHandleFromWindowName(const std::string& windowName) {
    HWND window = FindWindowA(nullptr, windowName.c_str());
    if (!window) {
        throw std::invalid_argument("Window not found");
    }
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    return OpenProcess(PROCESS_ALL_ACCESS,
                       FALSE,
                       processId);
}

void Injection::inject(const std::string& dllPath, const std::string& windowName) {
    HANDLE processHandle = getProcessHandleFromWindowName(windowName);
    if (!processHandle)
        throw std::invalid_argument("Process not found");

    LPVOID dllPathAddress = VirtualAllocEx(processHandle,nullptr,dllPath.size() + 1,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
    if (!dllPathAddress)
        throw std::invalid_argument("VirtualAllocEx Failed");

    if (WriteProcessMemory(processHandle,dllPathAddress,dllPath.c_str(),dllPath.size() + 1,nullptr))
        throw std::invalid_argument("WriteProcessMemory Failed");

    HANDLE thread = CreateRemoteThread(processHandle,nullptr,0,(LPTHREAD_START_ROUTINE) LoadLibraryA,dllPathAddress,0,nullptr);
    if (!thread)
        throw std::invalid_argument("CreateRemoteThread Failed");

    if (WaitForSingleObject(thread,INFINITE))
        throw std::invalid_argument("WaitForSingleObject Failed");

    CloseHandle(thread);
    if (VirtualFreeEx(processHandle,dllPathAddress,dllPath.size() + 1,MEM_RELEASE))
        throw std::invalid_argument("VirtualFreeEx Failed");

    CloseHandle(processHandle);
}

static std::vector<std::string> split(std::string str, const std::string& delim) {
    std::vector<std::string> split;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        split.push_back(token);
        str.erase(0, pos + delim.length());
    }
    split.push_back(str);
    return split;
}

std::vector<std::string> Injection::getMinecraftVersions() {
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
}