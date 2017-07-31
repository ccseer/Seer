#include "ckeyboardhookcontroler.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <qt_windows.h>
#include <QApplication>
#include "qthread.h"

#pragma comment(lib, "user32.lib")

bool IsAvailableClassName();
bool IsListaryVisible();

HHOOK g_hook;

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode,
                                        WPARAM wParam,
                                        LPARAM lParam);

bool CKeyboardHookControler::monitor_del_key = IsDelFileConfirmEnabled();

CKeyboardHookControler::CKeyboardHookControler(QObject *parent /*= 0*/)
    : QObject(parent), hooking(true)
{
    InitHook();
    UpdateHookKey();
}

void CKeyboardHookControler::InitHook()
{
    g_hook = SetWindowsHookEx(
        // 钩子的拦截消息类型，选择钩子程序的拦截范围，具体值参考文章结尾的消息类型;
        WH_KEYBOARD_LL,
        //消息的回调函数地址，一般是填函数名;
        MyLowLevelKeyBoardProc,
        //钩子函数所在的实例的句柄。对于线程钩子，该参数为NULL；对于系统钩子，该参数为钩子函数所在的DLL句柄。在dll中可通过AfxInitExtensionModule(MousehookDLL,
        // hInstance)获得DLL句柄。
        NULL,
        // 钩子所监视的线程的线程号，可通过GetCurrentThreadId()获得线程号。对于全局钩子，该参数为NULL(或0)
        0);

    if (!g_hook) {
        emit SigHookFailed();
    }
}

CKeyboardHookControler::~CKeyboardHookControler()
{
    UnhookWindowsHookEx(g_hook);
}

void CKeyboardHookControler::UpdateHookKey()
{
    m_spcae.alt   = g_pini->GetIni(Ini::key_alt_b).toBool();
    m_spcae.shift = g_pini->GetIni(Ini::key_shift_b).toBool();
    m_spcae.ctrl  = g_pini->GetIni(Ini::key_ctrl_b).toBool();

    m_spcae.alt_f   = g_pini->GetIni(Ini::key_alt_f_b).toBool();
    m_spcae.shift_f = g_pini->GetIni(Ini::key_shift_f_b).toBool();
    m_spcae.ctrl_f  = g_pini->GetIni(Ini::key_ctrl_f_b).toBool();

    m_spcae.is_fs_valid = true;
    if (m_spcae.alt == m_spcae.alt_f && m_spcae.shift == m_spcae.shift_f
        && m_spcae.ctrl == m_spcae.ctrl_f) {
        m_spcae.is_fs_valid = false;
    }
}

bool CheckModifiers(const CKeyboardHookControler::HookModifiers &m)
{
    return ((bool(0xF0 & GetKeyState(VK_MENU)) == m.alt)
            && (bool(0xF0 & GetKeyState(VK_SHIFT)) == m.shift)
            && (bool(0xF0 & GetKeyState(VK_CONTROL)) == m.ctrl));
}

bool CheckModifiersFullScreen(const CKeyboardHookControler::HookModifiers &m)
{
    if (!m.is_fs_valid) {
        return false;
    }
    return ((bool(0xF0 & GetKeyState(VK_MENU)) == m.alt_f)
            && (bool(0xF0 & GetKeyState(VK_SHIFT)) == m.shift_f)
            && (bool(0xF0 & GetKeyState(VK_CONTROL)) == m.ctrl_f));
}
// bool CheckModifiersWin()
//{
//    return (((bool(0xF0 & GetKeyState(VK_LWIN)) == true)
//             || (bool(0xF0 & GetKeyState(VK_RWIN)) == true)));
//}

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // WPARAM is WM_KEYDOWn, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP
    if (nCode >= 0 && g_pkeyboard->hooking == true && WM_KEYUP == wParam) {
        // LPARAM is the key information
        //判断是否还有其他按键被按下. 有就不响应
        KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT *)lParam);
        switch (key.vkCode) {
            case VK_SPACE: {
                // win 8 , win 10 切换输入法
                const auto &m = g_pkeyboard->GetSpaceMods();
                if (IsAvailableClassName() && !IsListaryVisible()) {
                    if (m.is_fs_valid) {
                        if (CheckModifiersFullScreen(m)) {
                            emit g_pkeyboard->SigKeyPressed(true);
                        }
                        else if (CheckModifiers(m)) {
                            emit g_pkeyboard->SigKeyPressed(false);
                        }
                    }
                    else {
                        if (CheckModifiers(m)) {
                            emit g_pkeyboard->SigKeyPressed(false);
                        }
                    }
                }
                break;
            }
            case VK_DELETE: {
                if (g_pkeyboard->monitor_del_key) {
                    emit g_pkeyboard->SigDeleteKeyPressed();
                }
                break;
            }
        }
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

/************************************************************************/
/*                          判断空格                                    */
/************************************************************************/

bool IsAvailableClassName()
{
    if (HWND activeWindowHandle = GetForegroundWindow()) {
        CONST DWORD activeWindowThread
            = GetWindowThreadProcessId(activeWindowHandle, nullptr);
        // DWORD thisWindowThread =
        // GetWindowThreadProcessId(getHWNDForWidget(widget), nullptr);
        CONST DWORD thisWindowThread = (DWORD)QThread::currentThreadId();
        if (AttachThreadInput(activeWindowThread, thisWindowThread, true)) {
            // Retrieves the handle to the window that has the keyboard focus
            if (HWND h_focused = GetFocus()) {
                if (AttachThreadInput(activeWindowThread, thisWindowThread,
                                      false)) {
                    wchar_t classname_focused[MAX_PATH];
                    if (GetClassName(h_focused, LPTSTR(classname_focused),
                                     MAX_PATH)) {
                        if (HWND h_parent = GetParent(h_focused)) {
                            wchar_t classname_parent[MAX_PATH];
                            if (GetClassName(h_parent, LPTSTR(classname_parent),
                                             MAX_PATH)) {
                                if (QString::fromWCharArray(classname_parent)
                                        == "SearchEditBoxWrapperClass"
                                    || QString::fromWCharArray(
                                           classname_focused)
                                           == "Edit") {
                                    return false;
                                }
                            }
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsListaryVisible()
{
    HWND handle = NULL;
    if (handle = FindWindowEx(NULL, NULL, L"Listary_WidgetWin_0", NULL)) {
        return IsWindowVisible(handle);
    }
    if (handle = FindWindowEx(NULL, NULL, L"Listary_WidgetWin_1", NULL)) {
        return IsWindowVisible(handle);
    }
    return false;
}
