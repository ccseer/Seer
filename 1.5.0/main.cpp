#include <QApplication>
#include "cappcrashhander.h"
#include "capplication.h"
#include "ccommandhandler.h"
#include "cglobaldata.h"
#include "cmain.h"
#include "cscreenadapter.h"
#include "cthememanager.h"
#include "ctranslationini.h"
#include "settingshelper.h"
#include "stable.h"

#include <QFont>
#include <QFontDatabase>
#include <QGlyphRun>
#include <QIcon>
#include <QPainter>
#include <QRawFont>
#include "qdatetime.h"

void InitApp();
void InitOutPut();

int main(int argc, char* argv[])
{
#ifdef Q_OS_WIN
    SetUnhandledExceptionFilter(
        (LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    CApplication app(argc, argv, true);
    app.setWindowIcon(QIcon("://icons/32.png"));
    // If this is a secondary instance
    if (app.isSecondary()) {
        return 0;
    }

    // init be4 CommandHander
    /// must be4 all the other code
    InitApp();
    InitOutPut();

    bool is_tr_ok = Translation_Ini::UpdateTranslation();

    CMain m;
    CCommandHandler cmd;
    app.connect(&app, &CApplication::SigNewIns, &cmd,
                &CCommandHandler::SlotNewInstanseActivated);
    m.connect(&cmd, &CCommandHandler::SigDuplicatedIns, &m,
              &CMain::DuplicatedIns);
    m.connect(&cmd, &CCommandHandler::SigHideWnd, &m, &CMain::HideWindow);
    m.connect(&cmd, &CCommandHandler::SigShakeWnd, &m, &CMain::ShakeWindow);
    m.connect(&cmd, &CCommandHandler::SigPreviewAFile, &m,
              &CMain::ActiveWindow);
    m.connect(&cmd, &CCommandHandler::SigShowTrayIcon, &m,
              &CMain::ShowTrayIcon);

    cmd.Parse(app.arguments());

    if (!is_tr_ok) {
        QTimer::singleShot(0, &m, &CMain::SetI18nFailed);
    }

    return app.exec();
}

void InitApp()
{
    qApp->setOrganizationName("Corey");
    qApp->setOrganizationDomain(web_home);
    qApp->setApplicationName(theAppName);
    qApp->setApplicationDisplayName(theAppName);
    qApp->setApplicationVersion(VERSION);

    qApp->setQuitOnLastWindowClosed(false);

    qApp->setEffectEnabled(Qt::UI_FadeMenu);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateTooltip);
    qApp->setEffectEnabled(Qt::UI_AnimateToolBox);

    if (QFontDatabase::addApplicationFont("://MaterialIcons-Regular.ttf") < 0) {
        qWarning() << "FontAwesome cannot be loaded !";
    }

    auto qss = CScreenAdapter::UpdateQSS(g_pstyle->GetQssFilePath(), 1);
    qApp->setStyleSheet(qss);

    QFont f(g_pini->GetIni(Ini::app_font_str).toString(), 9);
    f.setStyleStrategy(QFont::PreferAntialias);
    qApp->setFont(f);
}

void InitOutPut()
{
    //    qprintf<<"started:\t"VERSION"\t"+QDateTime::currentDateTime().toString("yyyy/MM/dd
    //    - hh:mm:ss.zzz")
    //          <<a.applicationFilePath()
    //         << GetAppTempPath();

    //    qprintf<<"------------------controls------------------";
    //    foreach(const auto & i, g_data->plugins)
    //    {
    //        qprintf << (i.name+"\t" + i.suffix.join(" ")+"\t" + i.para);
    //    }
    //    qprintf<<"------------------plugins------------------";
    //    foreach(const auto & i, g_data->controls.data)
    //    {
    //        qprintf <<(i.key+"\t" + i.exe+"\t" + i.args);
    //    }
    //    qprintf<<"------------------done------------------";
}
