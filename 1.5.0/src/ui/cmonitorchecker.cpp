#include "cmonitorchecker.h"
#include "cscreenadapter.h"
#include "stable.h"

#include <QApplication>

CMonitorChecker::CMonitorChecker(HWND wnd, QObject *parent)
    : QObject(parent), m_hwnd_main(wnd), m_ratio(1)
{
}

void CMonitorChecker::Init()
{
    Check();
    m_timer.setInterval(200);
    connect(&m_timer, &QTimer::timeout, this, &CMonitorChecker::Check);
}

void CMonitorChecker::Start()
{
    m_timer.start();
}

void CMonitorChecker::Stop()
{
    m_timer.stop();
}

bool CMonitorChecker::Check()
{
    if (auto h = MonitorFromWindow(m_hwnd_main, MONITOR_DEFAULTTONEAREST)) {
        MONITORINFOEX info;
        info.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(h, &info)) {
            const auto screen_name = QString::fromWCharArray(info.szDevice);
            foreach (auto i, qApp->screens()) {
                if (screen_name == i->name()) {
                    auto r = CScreenAdapter::getRatio(i);
                    if (r != m_ratio) {
                        m_ratio = r;
                        emit ratioChanged(m_ratio);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
