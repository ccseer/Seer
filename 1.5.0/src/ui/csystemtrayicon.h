#ifndef CSYSTEMTRAYICON_H
#define CSYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class QMenu;
class QAction;

class CSystemTrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    CSystemTrayIcon(QObject *parent = 0);
    ~CSystemTrayIcon();

    void SetEnableMenu4Settings(bool e);
    void SetEnableMenu4Register(bool e);
    void SetEnableMenu4All(bool e);
    void SetEnableMenu4Quit(bool e);

    void ResetTrayIconPic();
    void ResetTrayIconVisible();

private:
    void InitMenu();

    QAction *m_act_settings;
    QAction *m_act_quit;
    QAction *m_act_reg;

    QMenu *m_pmenu;

private:
signals:
    void SigQuit();
    void SigQuerySettings();
    void SigQueryAbout();
    void SigUpdateRequest();
    void SigHelp();
    void SigRegister();
    void SigStayontop(bool);

    void SigOpenUrlFailed();

private slots:
    void QuitTray();
    void OpenUrl();
    void SlotClicked(QSystemTrayIcon::ActivationReason why);
};

#endif  // CSYSTEMTRAYICON_H
