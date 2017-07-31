#include "csystemtrayicon.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"

#include <QApplication>
#include <QMenu>
#include <QSysInfo>
#include <QTimer>
#include "qdesktopservices.h"
#include "qurl.h"

CSystemTrayIcon::CSystemTrayIcon(QObject *parent) : QSystemTrayIcon(parent)
{
    // windows 7 & below uses color verison
    ResetTrayIconPic();

    InitMenu();
    setContextMenu(m_pmenu);
    setToolTip(AppName_char);
    connect(this, &CSystemTrayIcon::activated, this,
            &CSystemTrayIcon::SlotClicked);
    if (!g_pini->GetIni(Ini::tray_icon_hide_b).toBool()) {
        QTimer::singleShot(0, this, &CSystemTrayIcon::show);
    }
    else {
        setVisible(false);
    }
}

CSystemTrayIcon::~CSystemTrayIcon()
{
    delete m_pmenu;
}

void CSystemTrayIcon::ResetTrayIconPic()
{
    auto icon_url = "://icons/32.png";
    if (g_pini->GetIni(Ini::tray_icon_use_white_b).toBool()) {
        // windows 8 & above uses white verison
        icon_url = "://icons/logo_white.png";
    }
    setIcon(QIcon{icon_url});
}

void CSystemTrayIcon::ResetTrayIconVisible()
{
    auto h = g_pini->GetIni(Ini::tray_icon_hide_b).toBool();
    setVisible(!h);
}

void CSystemTrayIcon::InitMenu()
{
    m_pmenu   = new QMenu(tr("Menu"));
    m_act_reg = m_pmenu->addAction(tr("Register"), this, SIGNAL(SigRegister()));
    m_pmenu->addSeparator();
    auto ac = m_pmenu->addAction(tr("Stay on top"), this,
                                 SIGNAL(SigStayontop(bool)));
    ac->setCheckable(true);
    ac->setChecked(g_pini->GetIni(Ini::stay_on_top_b).toBool());

    m_act_settings
        = m_pmenu->addAction(tr("Settings"), this, SIGNAL(SigQuerySettings()));

    QMenu *menu = m_pmenu->addMenu(tr("About"));
    {
        menu->addAction(tr("About Seer"), this, SIGNAL(SigQueryAbout()));
        menu->addAction(tr("Help"), this, SIGNAL(SigHelp()));
        menu->addAction(tr("Found a bug?"), this, SLOT(OpenUrl()));
        menu->addSeparator();
        menu->addAction(tr("Check for updates"), this,
                        SIGNAL(SigUpdateRequest()));
    }
    m_pmenu->addSeparator();
    m_act_quit = m_pmenu->addAction(tr("Quit"), this, SLOT(QuitTray()));
}

void CSystemTrayIcon::QuitTray()
{
    hide();
    emit SigQuit();
}

void CSystemTrayIcon::SetEnableMenu4Settings(bool e)
{
    SetEnableMenu4All(e);
    m_act_settings->setEnabled(true);
}

void CSystemTrayIcon::SetEnableMenu4Register(bool e)
{
    SetEnableMenu4All(e);
    m_act_quit->setEnabled(true);
    m_act_reg->setEnabled(true);

    // add menuitem which can link to website
    foreach (auto i, m_pmenu->actions()) {
        if (i->text() == web_home) {
            m_pmenu->removeAction(i);
            delete i;
            break;
        }
    }
    if (!e) {
        auto act = new QAction(web_home, this);
        connect(act, &QAction::triggered, this, &CSystemTrayIcon::OpenUrl);
        m_pmenu->insertAction(m_pmenu->actions().first(), act);
    }
}

void CSystemTrayIcon::SetEnableMenu4All(bool e)
{
    foreach (auto i, m_pmenu->actions()) {
        i->setEnabled(e);
    }
}

void CSystemTrayIcon::SetEnableMenu4Quit(bool e)
{
    SetEnableMenu4All(e);
    m_act_quit->setEnabled(true);
}

void CSystemTrayIcon::SlotClicked(QSystemTrayIcon::ActivationReason why)
{
    if (why == QSystemTrayIcon::Context || why == QSystemTrayIcon::Trigger) {
        m_pmenu->popup(QCursor::pos());
    }
}

void CSystemTrayIcon::OpenUrl()
{
    if (auto act = (QAction *)sender()) {
        QString url = (act->text() == web_home) ? web_home : web_found_bug;
        if (!QDesktopServices::openUrl(QUrl(url))) {
            emit SigOpenUrlFailed();
        }
    }
}
