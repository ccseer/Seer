#include "cmainwndex.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <QPropertyAnimation>

CMainWndEx::CMainWndEx()
    : SleekWindow(theAppName),
#ifdef QXTGLOBALSHORTCUT
      m_pglobal_sc(NULL)
#endif
{
}

void CMainWndEx::Init()
{
    m_stayontop = g_pini->GetIni(Ini::stay_on_top_b).toBool();

    m_dpi = new CMonitorChecker(_hWnd, this);
    m_dpi->Init();
    connect(m_dpi, &CMonitorChecker::ratioChanged, this,
            &CMainWndEx::UpdateDPI);

    m_pwnd = new CMainWnd(m_dpi);
    connect(m_pwnd, &CMainWnd::SigHideWnd, this, &CMainWndEx::HideWnd);
    connect(m_pwnd, &CMainWnd::SigAfterLoaded, this, &CMainWndEx::AfterLoaded);
    connect(m_pwnd, &CMainWnd::SigBe4Load, this, &CMainWndEx::Be4Load);
    connect(m_pwnd, &CMainWnd::SigMaximizeWnd, this, &CMainWndEx::MaxmizeWnd);
    connect(m_pwnd, &CMainWnd::SigWaggleWnd, this, &CMainWndEx::WaggleWnd);
    connect(m_pwnd, &CMainWnd::SigTitleMouseEvt, this,
            &CMainWndEx::HandleTitleBarMouseEvt);
    QHBoxLayout *lay = new QHBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_pwnd);
    setLayout(lay);

    connect(this, &CMainWndEx::UpdateShortcuts, m_pwnd,
            &CMainWnd::UpdateShortcuts);

    // init sz
    UpdateDPI(m_dpi->ratio());
    centerPrimaryScreen();
    m_last_rt.rt = geometry();
}

bool CMainWndEx::IsSameSourceFile(const QString &p)
{
    return m_pwnd->IsSameSourceFile(p);
}

bool CMainWndEx::RespondDClick(const QPoint &pt)
{
    return m_pwnd->RespondDClick(pt);
}

void CMainWndEx::Load(const QString &path, HWND last_hwnd)
{
    m_hwnd_triggered = last_hwnd;
    if (!m_stayontop) {
        SetWindowPos(_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
        SetWindowPos(_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }
    m_pwnd->Load(path);
}

void CMainWndEx::WaggleWnd()
{
    static QPropertyAnimation *ani
        = new QPropertyAnimation(this, "ani_pos", this);
    if (ani->state() == QAbstractAnimation::Running) {
        return;
    }

    const auto dis = 3 * 10 * m_dpi->ratio();
    const auto dur = 540;
    const auto x   = pos().x();
    const auto y   = pos().y();
    ani->setKeyValueAt(0, pos());
    ani->setKeyValueAt(0.166, QPoint(x - dis, y));
    ani->setKeyValueAt(0.166 * 3, QPoint(x + dis, y));
    ani->setKeyValueAt(0.166 * 5, QPoint(x - dis, y));
    ani->setKeyValueAt(1, QPoint(x, y));
    ani->setDuration(dur);
    ani->start();
}

void CMainWndEx::Be4Load()
{
    // screen
    if (g_data->is_resized_by_user) {
        // 大小被改变后不再调整屏幕位置
    }
    else {
        //有值 , 默认主屏幕 ;
        const auto scr = GetTriggerScreen();

        if (scr != g_data->screen) {
            g_data->screen = scr;

            if (g_data->is_resized_by_user = m_last_rt.is_keep_rt) {
                return;
            }

            const auto rt_screen = scr->availableGeometry();
            const int screen_x   = rt_screen.width();
            const int screen_y   = rt_screen.height();
            move(QPoint(screen_x / 2 - width() / 2 + rt_screen.x(),
                        screen_y / 2 - height() / 2 + rt_screen.y()));
        }
    }
}

void CMainWndEx::SwitchWindow()
{
    if (m_pwnd->isActiveWindow()) {
        if (m_hwnd_triggered) {
            SetForegroundWindow(m_hwnd_triggered);
        }
    }
    else {
        m_pwnd->activateWindow();
    }
}

void CMainWndEx::AfterLoaded(const QSize &sz)
{
    if (m_last_rt.is_keep_rt) {
        return;
    }

    // update size
    ushort res_wid = 0;
    ushort res_hei = 0;
    {
        const auto sz_min            = GetMinWndSz(m_dpi->ratio());
        const QSize sz_available_max = GetMaxContentSize(g_data->screen);
        const QSize sz_res           = sz;
        if (sz_res.width() <= sz_min.width()) {
            res_wid = sz_min.width();
        }
        else {
            res_wid = qMin(sz_res.width(), sz_available_max.width());
        }
        if (sz_res.height() <= sz_min.height()) {
            res_hei = sz_min.height();
        }
        else {
            res_hei = qMin(sz_res.height(), sz_available_max.height());
        }
    }

    // set window center pos
    {
        QPoint pos_center_be4 = this->geometry().center();
        if (g_data->screen && m_last_wnd_pos.contains(g_data->screen)) {
            pos_center_be4 = m_last_wnd_pos.value(g_data->screen);
            m_last_wnd_pos.remove(g_data->screen);
        }
        auto rt = QRect(pos(), QSize(res_wid, res_hei));
        rt.moveCenter(pos_center_be4);
        setGeometry(rt);
    }

    // if wnd is out of desktop , put it inside
    {
        //以所有屏幕大小边界
        QRect rt_screen(0, 0, 0, 0);
        foreach (auto i, qApp->screens()) {
            rt_screen = rt_screen.united(i->availableGeometry());
        }

        const QPoint &p = pos();
        int x           = p.x();
        if (p.x() < rt_screen.x()) {
            x = rt_screen.x();
        }
        else if (p.x() + width() > rt_screen.width() + rt_screen.x()) {
            x = rt_screen.width() + rt_screen.x() - width();
        }

        int y = p.y();
        if (p.y() < rt_screen.y()) {
            y = rt_screen.y();
        }
        else if (p.y() + height() > rt_screen.height() + rt_screen.y()) {
            y = rt_screen.height() + rt_screen.y() - height();
        }

        if (p != QPoint{x, y}) {
            move(x, y);
        }
    }
    if (!m_dpi->Check()) {
        // check 只在 dpi 不相同的时候发信号。
        // 但是  contentWnd 里的内容都是按照 1 来显示的。加载时不知道具体 ratio
        // 所以这里 发送一个信号给 contents，如果不是 1 就 UPDATEDPI

        // getsize
    }
}

void CMainWndEx::HideWnd()
{
    emit SigMainWndAbout2Hide();

    const QRect rt = geometry();
    if (g_data->screen) {
        m_last_wnd_pos.insert(g_data->screen, rt.center());
    }
    if (m_last_rt.is_keep_rt) {
        m_last_rt.rt = rt;
    }

    hide();
    m_pwnd->CleanWnd();
    m_dpi->Stop();

    const auto rt_min = GetMinWndSz(m_dpi->ratio());
    setGeometry(rt.x() + (rt.width() - rt_min.width()) / 2,
                rt.y() + (rt.height() - rt_min.height()) / 2, rt_min.width(),
                rt_min.height());

#ifdef QXTGLOBALSHORTCUT
    // m_pglobal_sc
    cfree(m_pglobal_sc);
#endif
}

void CMainWndEx::Display()
{
    g_data->is_resized_by_user = false;

    m_last_rt.is_keep_rt = g_pini->GetIni(Ini::is_keep_wnd_sz_pos_b).toBool();
    if (m_last_rt.is_keep_rt) {
        setGeometry(m_last_rt.rt);
    }

    show();
    m_pwnd->ShowWnd();

    m_dpi->Check();

#ifdef QXTGLOBALSHORTCUT
    // m_pglobal_sc
    cfree(m_pglobal_sc);
    const auto key = g_pini->GetIni(Ini::key_get_focus_str).toString();
    if (!key.isEmpty()) {
        m_pglobal_sc = new QxtGlobalShortcut(QKeySequence(key), this);
        connect(m_pglobal_sc, &QxtGlobalShortcut::activated, this,
                &CMainWndEx::SwitchWindow);
    }
#endif
}

void CMainWndEx::Stayontop(bool a)
{
    HideWnd();
    m_stayontop = a;
    g_pini->SetIni(Ini::stay_on_top_b, a);
}

void CMainWndEx::UpdateDPI(qreal r)
{
    const auto c      = geometry().center();
    const auto min_sz = GetMinWndSz(r);
    const auto cur_sz = geometry();
    auto res_geo      = geometry();
    if (this->width() < min_sz.width()) {
        res_geo.setWidth(min_sz.width());
    }
    if (this->height() < min_sz.height()) {
        res_geo.setHeight(min_sz.height());
    }
    if (res_geo != cur_sz) {
        res_geo.moveCenter(c);
        setGeometry(res_geo);
    }

    setMinimumSize(min_sz.width(), min_sz.height());
    auto new_qss = CScreenAdapter::UpdateQSS(g_pstyle->GetQssFilePath(), r);
    this->setStyleSheet(new_qss);
    m_pwnd->UpdateDPI();
}

void CMainWndEx::HandleTitleBarMouseEvt(bool is_press)
{
    if (is_press) {
        m_dpi->Start();
        this->MoveWnd();
    }
    else {
        m_dpi->Stop();
    }
}
