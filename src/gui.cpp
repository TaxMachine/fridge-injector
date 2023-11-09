//
// Created by TaxMachine on 2023-11-08.
//

#include "gui.hpp"

#include <windows.h>
#include <windowsx.h>

#include

static LRESULT CALLBACK DragDropProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static OPENFILENAME ofn;
    static char filename[MAX_PATH];
    switch (msg) {
        case WM_LBUTTONDOWN: {
            SetCapture(hwnd);
            return 0;
        }

        case WM_MOUSEMOVE: {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            SetWindowPos(hwnd, nullptr, cursorPos.x, cursorPos.y, 0, 0, SWP_NOSIZE);
            return 0;
        }

        case WM_LBUTTONUP: {
            ReleaseCapture();
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFilter = "DLL files (*.dll)\0*.dll\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = "Select DLL file";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if(GetOpenFileName(&ofn)) {
                MessageBox(hwnd, filename, "Selected file", MB_OK);

            }

            return 0;

        }

        default: break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);

}

void GUI::render() {
    HWND form = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            TEXT("static"),
            TEXT("Drag and Drop"),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            400,
            400,
            nullptr,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
    );

    // Create draggable label
    HWND label = CreateWindow(
            TEXT("static"),
            TEXT("Drag Me"),
            WS_VISIBLE | WS_CHILD,
            100,
            100,
            100,
            30,
            form,
            nullptr,
            nullptr,
            nullptr
    );

    // Handle drag and drop
    auto oldProc = (WNDPROC)SetWindowLongPtr(label, GWLP_WNDPROC, (LONG_PTR)DragDropProc);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}