#include "cmain.h"
#include "caboutwnd.h"
#include "capplication.h"
#include "ccomponent.h"
#include "cconfirmdlg.h"
#include "cglobaldata.h"
#include "ckeyboardhookcontroler.h"
#include "cmsgbox.h"
#include "cregdlg.h"
#include "csettingsdlg.h"
#include "csystemtrayicon.h"
#include "cthememanager.h"
#include "ctranslationini.h"
#include "explorer.h"
#include "global_headers.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <QDateTime>
#include <QNetworkReply>
#include <QProcess>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQuickWindow>
#include <QVersionNumber>
#include "QFile"
#include "qevent.h"

#ifdef QXTGLOBALSHORTCUT
#include "qxtglobalshortcut\qxtglobalshortcut.h"
#endif

#ifdef UPDATE
#include "winsparkle.h"
#endif

CMain::CMain(QObject *parent)
    : QObject(parent),
      m_ptray(new CSystemTrayIcon),
      m_pwnd_settings(NULL),
      m_pwnd_about(NULL),
      m_pwnd_reg(NULL)
{
    m_pwnd = new CMainWndEx;
    m_pwnd->Init();

    connect(m_ptray, &CSystemTrayIcon::SigQuit, this, &CMain::QuitApp);
    connect(m_ptray, &CSystemTrayIcon::SigQuerySettings, this,
            &CMain::SlotShowSettings);
    connect(m_ptray, &CSystemTrayIcon::SigHelp, this, &CMain::SlotShowHelpDlg);
    connect(m_ptray, &CSystemTrayIcon::SigQueryAbout, this,
            &CMain::SlotShowAboutDlg);
    connect(m_ptray, &CSystemTrayIcon::SigUpdateRequest, this,
            &CMain::CheckUpdatesWithUI);
    connect(m_ptray, &CSystemTrayIcon::SigOpenUrlFailed, this,
            &CMain::SlotOpenUrlFailed);
    connect(m_ptray, &CSystemTrayIcon::SigRegister, this,
            &CMain::SlotShowRegisterDlg);
    connect(m_ptray, &CSystemTrayIcon::SigStayontop, m_pwnd,
            &CMainWndEx::Stayontop);

    {
        //每次触发 load 后 检测是否开始 timer;
        //窗口隐藏后 关闭 timer;
        m_track_timer.setInterval(400);
        connect(&m_track_timer, &QTimer::timeout, this,
                &CMain::TimerTrackingFile, Qt::QueuedConnection);
        connect(m_pwnd, &CMainWndEx::SigMainWndAbout2Hide, &m_track_timer,
                &QTimer::stop);
    }

    qApp->installEventFilter(this);

    QTimer::singleShot(0, this, SLOT(DelayInit()));
}

CMain::~CMain()
{
    delete m_ptray;
    delete m_pwnd;

    // delete file
    const auto path = GetAppTempPath();
    if (!g_pini->GetIni(Ini::save_temp_folder_b).toBool()) {
        QDir(path).removeRecursively();
    }
    else {
        foreach (const QString &i,
                 QDir(path).entryList(QStringList() << "*.json")) {
            QFile::remove(path + i);
        }
        QDir(GetFilePropertyTempPath()).removeRecursively();
    }

#ifdef UPDATE
    win_sparkle_cleanup();
#endif
}

void CMain::QuitApp()
{
    m_pwnd->HideWnd();
    cfree(m_pwnd_settings);
    cfree(m_pwnd_about);
    //需要 等待 HideWnd 动画完成
    QTimer::singleShot(200, qApp, SLOT(quit()));
}

void CMain::TestGlobalFocusSC(const QString &key)
{
    if (key.isEmpty()) {
        return;
    }
#ifdef QXTGLOBALSHORTCUT
    QxtGlobalShortcut test_sc;
    if (!test_sc.setShortcut(QKeySequence(key))) {
        m_ptray->showMessage(tr("Register shortcut failed"),
                             key + tr(" is registered by other application."));
    }
#endif
}

bool CMain::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o);
    switch (e->type()) {
        case QEvent::MouseButtonDblClick:
            if (!g_data->path.isEmpty()) {
                auto ev = (QMouseEvent *)e;
                return m_pwnd->RespondDClick(ev->globalPos());
            }
            break;
        case QEvent::ContextMenu:
            return true;
        case QEvent::KeyRelease:
            if (auto ev = (QKeyEvent *)e) {
                if (ev->key() == Qt::Key_Escape) {
                    m_pwnd->HideWnd();
                    return true;
                }

                if (g_pini->GetIni(Ini::use_hook_key_close_b).toBool()
                    && ev->key() == Qt::Key_Space
                    && g_pini->GetIni(Ini::key_alt_b).toBool()
                           == ev->modifiers().testFlag(Qt::AltModifier)
                    && g_pini->GetIni(Ini::key_ctrl_b).toBool()
                           == ev->modifiers().testFlag(Qt::ControlModifier)
                    && g_pini->GetIni(Ini::key_shift_b).toBool()
                           == ev->modifiers().testFlag(Qt::ShiftModifier)) {
                    m_pwnd->HideWnd();
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

void CMain::DelayInit()
{
    connect(g_pkeyboard, &CKeyboardHookControler::SigHookFailed, this,
            &CMain::SlotHookFailed);
    connect(g_pkeyboard, &CKeyboardHookControler::SigKeyPressed, this,
            &CMain::SlotHookActived,
            // can not get the right explorer data if the hook is blocked;
            Qt::QueuedConnection);
    connect(g_pkeyboard, &CKeyboardHookControler::SigDeleteKeyPressed, this,
            &CMain::SlotHookDeleteActived,
            // can not get the right explorer data if the hook is blocked;
            Qt::QueuedConnection);

    CheckVersion();
    CheckPlugins();
    CheckIsLicensed();

    if (g_pini->GetIni(Ini::auto_update_b).toBool()) {
        QTimer::singleShot(60 * 1000, this, &CMain::CheckUpdates);
    }

    // is QxtGlobalShortcut key registered by other app
    TestGlobalFocusSC(g_pini->GetIni(Ini::key_get_focus_str).toString());
}

void CMain::Restart()
{
    cApp->RestartApp();
}

/************************************************************************/
/*                  active                                              */
/************************************************************************/
void CMain::SlotHookFailed()
{
    CMsgBox::MsgBox(tr("Error"),
                    tr("Monitoring 'Space' key failed, can't proceed."),
                    tr("Quit"), true);
    qprint << "Keyboard Hook Failed";
    QuitApp();
}

void CMain::SlotHookActived(bool full_scr)
{
    const QString path = GetFocusedItem();
    if (path.isEmpty()) {
        return;
    }

    //只调用一次
    static bool s_is_first_invoke = true;
    if (s_is_first_invoke) {
        s_is_first_invoke = false;
        if (!FirstInvokeUnlicensed()) {
            return;
        }
    }

    ActiveWindow(path);
    if (full_scr && m_pwnd->isVisible()) {
        g_data->is_resized_by_user = true;
        m_pwnd->setGeometry(g_data->screen->availableGeometry());
    }
}

void CMain::SlotHookDeleteActived()
{
    if (!m_pwnd->isVisible() || m_pwnd->isActiveWindow()) {
        return;
    }
    // enhance: 判断是否是在 资源管理器 中
    QTimer::singleShot(0, m_pwnd, &CMainWndEx::HideWnd);
}

void CMain::TimerTrackingFile()
{
    //当 窗口不显示的时候 隐藏
    if (!m_pwnd->isVisible()) {
        return m_track_timer.stop();
    }
    if (m_pwnd->isActiveWindow()) {
        return;
    }

    const auto path = GetFocusedItem();
    if (path.isEmpty()
        // 是否和当前触发的是同一个文件
        || path == g_data->path
        // 是否是插件转换前的文件
        || m_pwnd->IsSameSourceFile(path)) {
        return;
    }

    ActiveWindow(path);
}

void CMain::SetI18nFailed()
{
    m_ptray->showMessage(QObject::tr("Update language failed"),
                         QObject::tr("Please check your \"i18n\" folder."));
}

QString CMain::GetFocusedItem()
{
    auto res = Explorers::GetThePath();
    QFileInfo info(res);
    if (res.isEmpty() || !info.exists()) {
        return {};
    }

    if (info.isSymLink()) {
        res  = info.symLinkTarget();
        info = QFileInfo(res);
    }
    return info.absoluteFilePath();
}

void CMain::ActiveWindow(const QString &path)
{
    if (!m_pwnd->isVisible()) {
        CloseWnds(Help);
        m_pwnd->Display();
    }
    m_pwnd->Load(path, GetForegroundWindow());

    if (g_pini->GetIni(Ini::tracking_file_b).toBool()
        && !m_track_timer.isActive()) {
        QTimer::singleShot(0, &m_track_timer, SLOT(start()));
    }
}

bool CMain::FirstInvokeUnlicensed()
{
    // 没有授权, 在试用期内.
    if (!CLicenseManager::CheckFromLocal()
        && CLicenseManager::CheckFreeTrial()) {
        // 过了试用期的无法触发 hook .
        CloseWnds(Settings);
        g_pkeyboard->hooking = false;
        m_ptray->SetEnableMenu4All(false);
        ShowFreeTrialDlg();
        g_pkeyboard->hooking = true;
        m_ptray->SetEnableMenu4All(true);
        return false;
    }
    return true;
}

/************************************************************************/
/*                           menu wnd                                   */
/************************************************************************/
void CMain::CloseWnds(const WndType &cur_type)
{
    if (cur_type != Help) {
        m_pwnd->HideWnd();
    }

    if (cur_type != About && m_pwnd_about) {
        m_pwnd_about->close();
    }
}

void DuplicateWnd(QWidget *wnd)
{
    wnd->activateWindow();
    wnd->raise();
    C::Ui::Ani::WaggleWnd(wnd, 420);
}

void CMain::SlotShowSettings()
{
    g_pkeyboard->hooking = false;
    m_ptray->SetEnableMenu4Settings(false);

    if (m_pwnd_settings) {
        DuplicateWnd(m_pwnd_settings);
        return;
    }

    CloseWnds(Settings);

    CSettingsDlg *wnd = NULL;
    m_pwnd_settings
        = CConfirmDlg::GetDlg<CSettingsDlg>(wnd, tr("Settings"), false);
    connect(m_pwnd_settings, &CConfirmDlg::SigConfirmed, this, [=]() {
        if (m_pwnd_settings && wnd) {
            const auto update_be4 = g_pini->GetIni(Ini::auto_update_b).toBool();
            const auto focus_key_be4
                = g_pini->GetIni(Ini::key_get_focus_str).toString();
            const auto font_str_be4
                = g_pini->GetIni(Ini::app_font_str).toString();

            wnd->SaveSettings();
            g_pkeyboard->UpdateHookKey();

            if (wnd->IsNeed2Restart()) {
                Restart();
                return;
            }
            if (font_str_be4 != g_pini->GetIni(Ini::app_font_str).toString()) {
                QFont f(g_pini->GetIni(Ini::app_font_str).toString(), 9,
                        QFont::Normal, false);
                f.setStyleStrategy(QFont::PreferAntialias);
                qApp->setFont(f);
            }
            if (update_be4 != g_pini->GetIni(Ini::auto_update_b).toBool()
                && g_pini->GetIni(Ini::auto_update_b).toBool() == true) {
                QTimer::singleShot(0, this, &CMain::CheckUpdates);
            }
            if (focus_key_be4
                != g_pini->GetIni(Ini::key_get_focus_str).toString()) {
                TestGlobalFocusSC(
                    g_pini->GetIni(Ini::key_get_focus_str).toString());
            }
            m_ptray->ResetTrayIconPic();
            m_ptray->ResetTrayIconVisible();
            emit m_pwnd->UpdateShortcuts();
        }
    });
    connect(wnd, &CSettingsDlg::SigReopen, this, [=]() {
        m_pwnd_settings->close();
        Restart();
    });
    connect(m_pwnd_settings, &CConfirmDlg::finished, [=](int /*result*/) {
        cfree(m_pwnd_settings);

        m_ptray->SetEnableMenu4Settings(true);
        g_pkeyboard->hooking = true;
    });
    m_pwnd_settings->setProperty("_SettingsWnd", true);
    m_pwnd_settings->SetTitleVisible(false);
    m_pwnd_settings->SetBottomWndObjectName("settings_bottom_wnd");
    m_pwnd_settings->setWindowOpacity(1);
    m_pwnd_settings->SetCloseOnEscKey(false);
    m_pwnd_settings->show();
    wnd->Init(m_pwnd_settings->GetDPI());
}

void CMain::SlotShowAboutDlg()
{
    if (m_pwnd_about) {
        DuplicateWnd(m_pwnd_about);
        return;
    }

    CloseWnds(About);

    CAboutWnd *wnd = NULL;
    m_pwnd_about   = CParentDlg::GetDlg<CAboutWnd>(wnd, "", false);
    connect(m_pwnd_about, &CParentDlg::finished,
            [=]() { cfree(m_pwnd_about); });
    connect(wnd, &CAboutWnd::SigOpenUrlFailed, this, &CMain::SlotOpenUrlFailed);

    m_pwnd_about->setProperty("_AboutWnd", true);
    m_pwnd_about->SetCloseOnEscKey(true);
    m_pwnd_about->SetTitleVisible(false);
    m_pwnd_about->show();
    wnd->UpdateDPI(m_pwnd_about->GetDPI()->ratio());
    connect(m_pwnd_about->GetDPI(), &CScreenAdapter::ratioChanged, wnd,
            &CAboutWnd::UpdateDPI);
}

void CMain::SlotShowHelpDlg()
{
    CloseWnds(Help);

    if (m_pwnd->isVisible() && g_data->path == g_path_help) {
        m_pwnd->WaggleWnd();
        return;
    }

    m_pwnd->Display();
    m_pwnd->Load(g_path_help, 0);
}

void CMain::SlotShowRegisterDlg()
{
    g_pkeyboard->hooking = false;
    m_ptray->SetEnableMenu4Register(false);

    if (m_pwnd_reg) {
        DuplicateWnd(m_pwnd_reg);
        return;
    }

    CloseWnds(Reg);

    CRegDlg *wnd = NULL;
    m_pwnd_reg = CParentDlg::GetDlg<CRegDlg>(wnd, tr("Seer License Activation"),
                                             false);
    connect(wnd, &CRegDlg::SigClose, this, [=]() {
        cfree(m_pwnd_reg);
        const auto is_usable = CLicenseManager::IsUsable();
        m_ptray->SetEnableMenu4Register(is_usable);
        g_pkeyboard->hooking = is_usable;
    });

    m_pwnd_reg->setProperty("_RegWnd", true);
    m_pwnd_reg->SetTitleCloseVisible(false);
    m_pwnd_reg->SetTitleLeft();
    m_pwnd_reg->SetTitleObjectName("reg_title_wnd");
    m_pwnd_reg->SetTitleMargin(QMargins(3 * 3, 3, 3, 0));
    m_pwnd_reg->SetCloseOnEscKey(false);
    connect(
        m_pwnd_reg->GetDPI(), &CScreenAdapter::ratioChanged, wnd, [=](qreal r) {
            wnd->UpdateDPI(r);
            m_pwnd_reg->SetTitleMargin(QMargins(3 * 3 * r, 3 * r, 3 * r, 0));
        });
    m_pwnd_reg->show();
}

void CMain::SlotOpenUrlFailed()
{
    m_ptray->showMessage(AppName_char,
                         tr("The system default application not found."));
}

void CMain::ShowFreeTrialDlg(bool is_expired /* = false*/)
{
    QQmlEngine engine(this);
    QQmlComponent component(&engine);
    engine.rootContext()->setContextProperty("m_manager", this);
    engine.rootContext()->setContextProperty("m_ratio", 1);
    auto url_path
        = is_expired ? "qrc:/qml/overdue.qml" : "qrc:/qml/freetrial.qml";
    component.loadUrl(QUrl(url_path));
    if (!component.isReady()) {
        elprint;
        return;
    }

    // The ownership of the returned object instance is transferred to the
    // caller.
    QScopedPointer<QObject> com_obj(component.create(engine.rootContext()));
    if (com_obj.isNull()) {
        return;
    }
    if (auto scr = qobject_cast<QQuickWindow *>(com_obj.data())->screen()) {
        auto r = CScreenAdapter::getRatio(scr);
        QQmlProperty::write(com_obj.data(), "m_ratio", r);
    }

    QEventLoop l;
    connect(this, &CMain::qmlWindowClosed, &l, &QEventLoop::quit);
    l.exec();
}
/************************************************************************/
/*                     Command                                          */
/************************************************************************/
void CMain::HideWindow()
{
    if (m_pwnd->isVisible()) {
        m_pwnd->HideWnd();
    }
}

void CMain::ShakeWindow()
{
    if (m_pwnd->isVisible()) {
        m_pwnd->WaggleWnd();
    }
}

void CMain::DuplicatedIns()
{
    //如果 Mainwindow 在显示 , 激活窗口  , otherwise , tray 弹出提示
    m_ptray->showMessage(AppName_char, QObject::tr("Seer is already running."));
}

void CMain::ShowTrayIcon()
{
    g_pini->SetIni(Ini::tray_icon_hide_b, false);
    QTimer::singleShot(0, m_ptray, &CSystemTrayIcon::ResetTrayIconVisible);
}

/************************************************************************/
/*                           checking                                   */
/************************************************************************/
void CMain::CheckVersion()
{
    g_pini->SetIni(Ini::app_path_str, qApp->applicationFilePath());

    const QString ver_ini = g_pini->GetIni(Ini::version_str).toString();
    //第一次运行
    if (ver_ini.isEmpty()) {
        SlotShowHelpDlg();
        m_ptray->showMessage("Seer", tr("Application started."),
                             QSystemTrayIcon::Information, 30000);
    }
    else {
        // 同一版本
        if (ver_ini == VERSION) {
            return;
        }

        const auto ver_ini_list = ver_ini.split(".");
        const auto ver_cur_list = QString(VERSION).split(".");
        if (ver_cur_list.size() != ver_ini_list.size()
            && ver_cur_list.size() != 3) {
            elprint;
        }
        else {
            const QVersionNumber version_ini(ver_ini_list.at(0).toInt(),
                                             ver_ini_list.at(1).toInt(),
                                             ver_ini_list.at(2).toInt());
            const QVersionNumber version_cur(ver_cur_list.at(0).toInt(),
                                             ver_cur_list.at(1).toInt(),
                                             ver_cur_list.at(2).toInt());
            //更新后第一次运行
            if (version_ini < version_cur) {
                // clean, 新版本第一次运行, 清理上一个版本的垃圾
                m_ptray->showMessage(tr("Like Seer?"),
                                     tr("Please share with your friends."));
            }
        }
    }
    g_pini->SetIni(Ini::version_str, QString(VERSION));
}

void CMain::CheckPlugins()
{
    static bool is_checked = false;
    if (is_checked) {
        return;
    }
    is_checked = true;

    QNetworkAccessManager *network = new QNetworkAccessManager(this);
    connect(network, &QNetworkAccessManager::finished, network,
            [=](QNetworkReply *output) {
                if (output->error() == QNetworkReply::NoError) {
                    const auto res = output->readAll();
                    output->close();
                    if (!res.isEmpty()) {
                        g_pini->SetIni(Ini::plugins_data_all_bytes, res);
                        qprint << "plugin data checked";
                    }
                }
                else {
                    qprint << "plugin check error" << output->errorString();
                }
                network->deleteLater();
            });

    auto reply = network->get(QNetworkRequest(QUrl(web_plugin_update)));
    if (reply->error() != QNetworkReply::NoError) {
        network->deleteLater();
        qprint << "plugin check error" << reply->errorString();
    }
    // goto lambda slot
}

void UpdatesHelper(bool has_ui)
{
#ifdef UPDATE
    win_sparkle_set_appcast_url(web_update);
    win_sparkle_set_app_details(L"Corey", AppName_charL,
                                (wchar_t *)QString(VERSION).utf16());

    win_sparkle_init();
    win_sparkle_set_automatic_check_for_updates(0);
    has_ui ? win_sparkle_check_update_with_ui()
           : win_sparkle_check_update_without_ui();
#endif
}

void CMain::CheckUpdates()
{
    UpdatesHelper(false);
}

void CMain::CheckUpdatesWithUI()
{
    UpdatesHelper(true);
}

void CMain::CheckIsLicensed()
{
    // DelayInit ->  CheckIsLicensed -> Licensed    ->  passed  -> hook
    g_pkeyboard->hooking = false;
    m_ptray->SetEnableMenu4Register(false);
    const auto is_usable = CLicenseManager::IsUsable();
    g_pkeyboard->hooking = is_usable;
    m_ptray->SetEnableMenu4Register(is_usable);
    if (!is_usable) {
        g_pkeyboard->hooking = false;
        m_ptray->SetEnableMenu4Quit(false);
        ShowFreeTrialDlg(true);
        g_pkeyboard->hooking = is_usable;
        m_ptray->SetEnableMenu4Register(is_usable);
    }
}
