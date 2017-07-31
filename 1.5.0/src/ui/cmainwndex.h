#ifndef CMAINWNDEX_H
#define CMAINWNDEX_H

#include "cmainwnd.h"
#include "cmonitorchecker.h"
#include "cscreenadapter.h"
#include "libsleek/sleekwindow.h"

#ifdef QXTGLOBALSHORTCUT
#include "qxtglobalshortcut\qxtglobalshortcut.h"
#endif

class CMainWndEx : public SleekWindow {
    Q_OBJECT
    Q_PROPERTY(QPoint ani_pos READ pos WRITE move)

public:
    explicit CMainWndEx();
    void Init();

    bool IsSameSourceFile(const QString &p);

    bool RespondDClick(const QPoint &pt);

    void Load(const QString &path, HWND hwnd_last_triggered);

    void WaggleWnd();

    void AfterLoaded(const QSize &);

    void HideWnd();
    void Display();

    void Stayontop(bool a);

private:
    struct {
        bool is_keep_rt;
        QRect rt;
    } m_last_rt;

#ifdef QXTGLOBALSHORTCUT
    QxtGlobalShortcut *m_pglobal_sc;
#endif

    QHash<QScreen *, QPoint> m_last_wnd_pos;

    CMainWnd *m_pwnd;

    HWND m_hwnd_triggered;

    CMonitorChecker *m_dpi;

signals:
    void SigMainWndAbout2Hide();
    void UpdateShortcuts();

private slots:
    void UpdateDPI(qreal r);
    void HandleTitleBarMouseEvt(bool is_press);
    void Be4Load();
    void SwitchWindow();
};

#endif  // CMAINWNDEX_H
