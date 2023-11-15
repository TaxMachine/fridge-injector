//
// Created by TaxMachine on 2023-11-08.
//

#include "injection.hpp"

#include <regex>

#include "exceptions.hpp"
#include "utils.hpp"

#ifdef WIN32
    #include <windows.h>
    #include <psapi.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
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

void Injection::inject(const std::string& dllPath, const std::string& windowName) {
#ifdef WIN32
    const HANDLE& processHandle = getProcessHandleFromWindowName(windowName);
    if (!processHandle)
        throw InjectionException("Process not found");

    const LPVOID& dllPathAddress = VirtualAllocEx(
        processHandle,
        nullptr,
        dllPath.size() + 1,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);

    if (!dllPathAddress)
        throw InjectionException("VirtualAllocEx Failed");

    WriteProcessMemory(
        processHandle,
        dllPathAddress,
        dllPath.c_str(),
        dllPath.size() + 1,
        nullptr) == FALSE ?
            throw InjectionException("WriteProcessMemory Failed") : 0;

    const HANDLE& thread = CreateRemoteThread(
        processHandle,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        dllPathAddress,
        0,
        nullptr);

    if (!thread)
        throw InjectionException("CreateRemoteThread Failed");

    WaitForSingleObject(thread,INFINITE) == WAIT_FAILED ?
        throw InjectionException("WaitForSingleObject Failed") : 0;

    CloseHandle(thread);

    VirtualFreeEx(processHandle,dllPathAddress,dllPath.size() + 1,MEM_RELEASE) == FALSE ?
        throw InjectionException("VirtualFreeEx Failed") : 0;

    CloseHandle(processHandle);
#elif __linux__
    throw NotImplementedException("Linux is not supported");
#endif
}

std::vector<MCInstance> Injection::getMinecraftVersions() {
#ifdef WIN32
    std::vector<MCInstance> versions;

    HWND window = FindWindowA(nullptr, nullptr);
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
            GetWindowThreadProcessId(window, &processId) ?
                throw InjectionException("GetWindowThreadProcessId Failed") : 0;
            instance.pid = static_cast<int>(processId);
            std::string javawPath{};
            const HANDLE& proc = OpenProcess(
            PROCESS_QUERY_INFORMATION,
            FALSE,
            instance.pid);

            if (!proc)
                throw InjectionException("OpenProcess Failed");

            GetProcessImageFileNameA(
                proc,
                javawPath.data(),
                256) == FALSE ?
                    throw InjectionException("GetProcessImageFileNameA Failed") : 0;

            instance.javawPath = javawPath;
            versions.push_back(instance);
        }
        window = GetWindow(window, GW_HWNDNEXT);
    }

    return versions;
#elif __linux__
    throw NotImplementedException("Linux is not supported");
#endif
}