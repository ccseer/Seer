- There are two ways to toggle Seer

  - Using `SendMessage`(*recommended*), written in [bool sendPath2Seer(HWND seer, LPCWSTR path)](https://github.com/ccseer/Seer/blob/master/ipc/ACustomApp/mainwindow.cpp)

  - Using command line: `/path/to/Seer.exe the_file_you_need_to_preview`

    - below is the **logic** code of finding Seer's full path in case you need it
    - ```c++
        const QString &getDopusrt()
        {
            static QString exe;
            if (!exe.isEmpty()) {
                return exe;
            }

            auto gfw = GetForegroundWindow();
            DWORD dw_pid;
            GetWindowThreadProcessId(gfw, &dw_pid);
            HANDLE hproc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                    FALSE, dw_pid);
            if (!hproc) {
                return exe;
            }

            TCHAR exe_w32[MAX_PATH] = {0};
            if (GetModuleFileNameEx(hproc, NULL, exe_w32, MAX_PATH) > 0) {
                exe = QString::fromWCharArray(exe_w32).trimmed();
            }
            else {
                // handle the failure
            }
            CloseHandle(hproc);
            return exe;
        }

      ```
