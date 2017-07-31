#include "explorer.h"
#include "ccomponent.h"
#include "stable.h"

#include "qt_windows.h"
#include "destop.h"

enum FocusedType { FT_Desktop, FT_Folder, FT_Dialog, FT_DO, FT_EVERYTHING };

QPoint g_point_desktop;
bool g_is_desktop;

QString Dispatch()
{
    static const QHash<QString, FocusedType> names{
        {"Progman", FT_Desktop},
        {"WorkerW", FT_Desktop},
        {"CabinetWClass", FT_Folder},
        {"#32770", FT_Dialog},
        //{"EVERYTHING", FT_EVERYTHING},
        {"dopus.lister", FT_DO}
        // res.insert("TTOTAL_CMD", s_name_tc);
    };

    static wchar_t name_fore[MAX_PATH];
    name_fore[0] = TEXT('\0');

    auto hwnd_focus = GetForegroundWindow();
    if (!GetClassName(hwnd_focus, LPTSTR(name_fore), MAX_PATH)) {
        return {};
    }
    const QString str_fore = QString::fromWCharArray(name_fore);

    if (!names.contains(str_fore)) {
        return {};
    }

    g_is_desktop = false;

    switch (names[str_fore]) {
        case FT_Desktop: {
            g_is_desktop = true;
            return Explorers::GetPathDesktop(true, g_point_desktop);
        }
        case FT_Folder: {
            return Explorers::GetPathDesktop(false, g_point_desktop);
        }
        case FT_Dialog: {
            if (FindWindowEx(hwnd_focus, nullptr, L"DUIViewWndClassName",
                             nullptr)
                != nullptr) {
                return Explorers::GetPathDlg();
            }
            return {};
        }
        case FT_DO: {
//            return Explorers::GetPathDO();
        }
            //        case FT_EVERYTHING: {
            //            return Explorers::GetPathEverything();
            //        }
    }

    elprint;
    return {};
}

bool Explorers::GetFileIconPosInDesktop(QPoint &pt)
{
    pt = g_point_desktop;
    return g_is_desktop;
}

QString Explorers::GetThePath()
{
    if (FAILED(CoInitialize(NULL))) {
        elprint;
        return {};
    }

    QString res = Dispatch();

    CoUninitialize();

    return res;
}
