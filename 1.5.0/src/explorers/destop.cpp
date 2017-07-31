#include "destop.h"
#include "stable.h"

#include <Shlwapi.h>
#include <atlbase.h>
#include <atlcomcli.h>
#include <exdisp.h>
#include <shlobj.h>
#include <shobjidl.h>

TCHAR g_path[MAX_PATH];
TCHAR g_item[MAX_PATH];
bool FindFolderView(REFIID riid, void **ppv, const bool is_desktop)
{
    CComPtr<IShellWindows> psw;
    if (FAILED(psw.CoCreateInstance(CLSID_ShellWindows))) {
        elprint;
        return false;
    }

    CComPtr<IDispatch> spdisp;
    CComPtr<IShellBrowser> spBrowser;
    CComPtr<IShellView> spView;

    //////////////////////////////////////////////////////////////////////////
    // explorer
    if (!is_desktop) {
        long sw_t = -1;
        if (S_OK != psw->get_Count(&sw_t)) {
            elprint;
            return false;
        }
        CComVariant v;
        V_VT(&v) = VT_I4;
        for (V_I4(&v) = 0; V_I4(&v) < sw_t; V_I4(&v)++) {
            if (SUCCEEDED(psw->Item(v, &spdisp))) {
                if (spdisp
                    && SUCCEEDED(
                           CComQIPtr<IServiceProvider>(spdisp)->QueryService(
                               SID_STopLevelBrowser, IID_PPV_ARGS(&spBrowser)))
                    && spBrowser
                    && SUCCEEDED(spBrowser->QueryActiveShellView(&spView))
                    && spView && SUCCEEDED(spView->QueryInterface(riid, ppv))) {
                    //判断是否是最上层窗口
                    HWND h;
                    if (FAILED(spView->GetWindow(&h))) {
                        elprint;
                        continue;
                    }
                    while (h) {
                        if (h == GetForegroundWindow()) {
                            return true;
                        }
                        h = GetParent(h);
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    // desktop
    else {
        CComVariant vtLoc(CSIDL_DESKTOP);
        CComVariant vtEmpty;
        long lhwnd;
        if (FAILED(psw->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd,
                                     SWFO_NEEDDISPATCH, &spdisp))) {
            elprint;
            return false;
        }
        if (spdisp
            && SUCCEEDED(CComQIPtr<IServiceProvider>(spdisp)->QueryService(
                   SID_STopLevelBrowser, IID_PPV_ARGS(&spBrowser)))
            && spBrowser && SUCCEEDED(spBrowser->QueryActiveShellView(&spView))
            && spView && SUCCEEDED(spView->QueryInterface(riid, ppv))) {
            return true;
        }
    }

    elprint;
    return false;
}

QString Explorers::GetPathDesktop(bool is_desktop, QPoint &trigger_point)
{
    trigger_point = {};
    g_item[0]     = TEXT('\0');
    CComPtr<IFolderView> spView;
    if (!FindFolderView(IID_PPV_ARGS(&spView), is_desktop)) {
        elprint;
        return {};
    }

    CComPtr<IShellFolder> spFolder;
    if (FAILED(spView->GetFolder(IID_PPV_ARGS(&spFolder)))) {
        elprint;
        return {};
    }

    LPITEMIDLIST pidl_item;
    int iFocus;
    if (SUCCEEDED(spView->GetFocusedItem(&iFocus))
        && SUCCEEDED(spView->Item(iFocus, &pidl_item))) {
        STRRET str;
        if (SUCCEEDED(spFolder->GetDisplayNameOf(pidl_item, SHGDN_FORPARSING,
                                                 &str))) {
            StrRetToBuf(&str, pidl_item, g_item, MAX_PATH);

            POINT pt;
            if (SUCCEEDED(spView->GetItemPosition(pidl_item, &pt))) {
                trigger_point = {pt.x, pt.y};
            }

            CoTaskMemFree(pidl_item);
            return QString::fromWCharArray(g_item);
        }
    }
    CoTaskMemFree(pidl_item);
    elprint;
    return {};
}
