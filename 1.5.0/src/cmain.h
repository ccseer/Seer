#ifndef CMAIN_H
#define CMAIN_H

#include "cmainwndex.h"
#include "qtimer.h"

#include <QNetworkAccessManager>

class CSystemTrayIcon;
class CConfirmDlg;
class CSettingsDlg;
class CParentDlg;

class CMain : public QObject {
    Q_OBJECT

    enum WndType { Settings, About, Reg, Help };

public:
    CMain(QObject* parent = 0);
    ~CMain();

protected:
    virtual bool eventFilter(QObject*, QEvent*);

private:
    void CheckUpdates();
    void CheckUpdatesWithUI();
    void CheckVersion();
    void CheckPlugins();
    void CheckIsLicensed();

    void CloseWnds(const WndType& cur_type);
    bool FirstInvokeUnlicensed();
    void ShowFreeTrialDlg(bool is_expired = false);

    QString GetFocusedItem();
    void TestGlobalFocusSC(const QString& key);
    void Restart();

    QTimer m_track_timer;

    CSystemTrayIcon* m_ptray;

    CMainWndEx* m_pwnd;
    CConfirmDlg* m_pwnd_settings;
    CParentDlg* m_pwnd_about;
    CParentDlg* m_pwnd_reg;

public:
signals:
    void qmlWindowClosed();

public slots:
    void QuitApp();
    void ActiveWindow(const QString& path);

    void SetI18nFailed();

    // command
    void ShowTrayIcon();
    void HideWindow();
    void ShakeWindow();
    void DuplicatedIns();

private slots:
    void DelayInit();
    void SlotShowSettings();
    void SlotShowAboutDlg();
    void SlotShowHelpDlg();
    void SlotShowRegisterDlg();

    void SlotOpenUrlFailed();

    void TimerTrackingFile();

    void SlotHookFailed();
    void SlotHookActived(bool full_scr = false);
    void SlotHookDeleteActived();
};

#endif  // CMAIN_H
