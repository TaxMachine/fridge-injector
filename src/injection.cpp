//
// Created by TaxMachine on 2023-11-08.
//

#include "injection.hpp"

#include <string>

static HANDLE getProcessHandleFromWindowName(const std::string& windowName) {
    HWND window = FindWindowA(nullptr, windowName.c_str());
    DWORD processId;
    GetWindowThreadProcessId(window, &processId);
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}

void Injection::inject(std::string dllPath, HANDLE processHandle) {
    LPVOID dllPathAddress = VirtualAllocEx(processHandle, nullptr, dllPath.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(processHandle, dllPathAddress, dllPath.c_str(), dllPath.size() + 1, nullptr);
    HANDLE thread = CreateRemoteThread(processHandle, nullptr, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, dllPathAddress, 0, nullptr);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    VirtualFreeEx(processHandle, dllPathAddress, dllPath.size() + 1, MEM_RELEASE);
    CloseHandle(processHandle);
}