#include "ccommandhandler.h"
#include "stable.h"
#include "utilities.h"

#include "qapplication.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qlogging.h"
#include "qtextstream.h"

// void CustomMessageHandler(QtMsgType type, const QMessageLogContext &context,
// const QString &msg);

CCommandHandler::CCommandHandler(QObject *parent /*= nullptr*/)
    : QObject(parent)
{
    // app.exe path
    // m_args.takeFirst();

    m_parser.setApplicationDescription("You know what it is.");

    m_parser.addPositionalArgument("path", "File to preview, optionally.",
                                   "[path]");

    QCommandLineOption opt_help(QStringList{"?"}, "Displays this help.");
    m_parser.addOption(opt_help);
    QCommandLineOption opt_version(QStringList{"v"},
                                   "Displays version information.");
    m_parser.addOption(opt_version);

    QCommandLineOption opt_i(QStringList{"t", "ticn"}, "Show tray icon");
    m_parser.addOption(opt_i);

    QCommandLineOption opt_hide(QStringList{"c", "cya"},
                                "Hide the preview window.");
    m_parser.addOption(opt_hide);
    QCommandLineOption opt_shake(QStringList{"s", "shake"},
                                 "Shake the preview window.");
    m_parser.addOption(opt_shake);
}

void CCommandHandler::Parse(const QStringList &args, bool is_first_ins)
{
    if (!m_parser.parse(args)) {
        qprint << m_parser.errorText();
        return;
    }

    if (m_parser.isSet("v")) {
        if (is_first_ins) {
            m_parser.showVersion();
        }
        else {
            qprint << "Application argument ignored: -v.\tExit Seer and try "
                      "again.\t[END]";
        }
        return;
    }
    if (m_parser.isSet("?")) {
        if (is_first_ins) {
            m_parser.showHelp();
        }
        else {
            qprint << "Application argument ignored: -?.\tExit Seer and try "
                      "again.\t[END]";
        }
        return;
    }

    if (m_parser.isSet("t")) {
        emit SigShowTrayIcon();
        qprint << "Application argument accepted: -t.  [END]";
        return;
    }

    if (m_parser.isSet("c")) {
        emit SigHideWnd();
        qprint << "Application argument accepted: -c.  [END]";
        return;
    }
    if (m_parser.isSet("s")) {
        emit SigShakeWnd();
        qprint << "Application argument accepted: -s.  [END]";
        return;
    }

    // target
    const auto target_args = m_parser.positionalArguments();
    if (target_args.isEmpty()) {
        qprint << "Application argument: [path] argument not found.  [END]";
        return;
    }
    const auto target = QFileInfo(target_args.first()).absoluteFilePath();
    if (!QFileInfo::exists(target)) {
        qprint << QString("Application argument error : %1 not found.  [END]")
                      .arg(target);
        return;
    }

    emit SigPreviewAFile(target);
}

void CCommandHandler::SlotNewInstanseActivated(const QStringList &args)
{
    // command
    if (args.count() == 1) {
        emit SigDuplicatedIns();
    }
    else {
        Parse(args, false);
    }
}
