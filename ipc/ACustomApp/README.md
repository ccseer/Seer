- There are two ways to toggle Seer

  - Use `SendMessage`, written in [bool sendPath2Seer(HWND seer, LPCWSTR path)](https://github.com/ccseer/Seer/blob/master/ipc/ACustomApp/mainwindow.cpp)

  - Use command line: `/path/to/Seer.exe the_file_you_need_to_preview`

    - below is the logic code of finding Seer's full path
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

            TCHAR exe_path[MAX_PATH] = {0};
            if (GetModuleFileNameEx(hproc, NULL, exe_path, MAX_PATH) > 0) {
                exe = QString::fromWCharArray(exe_path).trimmed();
            }
            else {
            }
            CloseHandle(hproc);
            return exe;
        }

      ```
