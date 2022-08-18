#ifndef WIN32_H
#define WIN32_H

#include <tchar.h>
#include <windows.h>

#include "seer_ipc.h"

#pragma comment(lib, "User32.lib")

bool invokeWithWin32()
{
    // make sure Seer is running
    if (HWND h = FindWindowEx(nullptr, nullptr, SEER_CLASS_NAME, nullptr)) {
        WCHAR path[MAX_PATH] = {0};
        // target file
        _tcscpy(path, L"C:\\D\\7z.exe");

        COPYDATASTRUCT cd;
        cd.cbData = MAX_PATH;
        cd.lpData = (LPVOID)path;
        cd.dwData = SEER_INVOKE_W32;
        auto err  = SendMessage(h, WM_COPYDATA, 0, (LPARAM)&cd);
        return SUCCEEDED(err);
    }
    else {
        // Seer is not running
    }
    return false;
}

#endif  // WIN32_H
