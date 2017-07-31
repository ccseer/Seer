#include "cplayprocess.h"
#include "stable.h"

CPlayProcess::CPlayProcess(QObject *parent)
    : QProcess(parent), m_is_killed(false)
{
    connect(
        this,
        // signal overload
        (void (QProcess::*)(int)) & QProcess::finished, [=]() {
            if (IsFinishedByKilled() || exitStatus() != QProcess::NormalExit) {
                emit SigFinishedByKilled();
                return;
            }
            emit SigFinishedNaturally();
        });
}

bool CPlayProcess::IsRunning()
{
    return (state() == QProcess::Running);
}

void CPlayProcess::QuitProcess()
{
    if (!IsRunning()) {
        return;
    }
    kill();
    waitForFinished(200);
    if (!IsRunning()) {
        return;
    }
    waitForFinished(-1);
}

void CPlayProcess::Command(const QString &str)
{
    write(str.toLocal8Bit() + "\n");
}

void CPlayProcess::kill()
{
    m_is_killed = true;
    QProcess::kill();
}

void CPlayProcess::start(const QString &program,
                         const QStringList &arguments,
                         OpenMode mode /*= ReadWrite*/)
{
    m_is_killed = false;
    QProcess::start(program, arguments, mode);
}

void CPlayProcess::start(const QString &program)
{
    m_is_killed = false;
    QProcess::start(program);
}

void CPlayProcess::start(OpenMode mode /*= ReadWrite*/)
{
    m_is_killed = false;
    QProcess::start(mode);
}

bool CPlayProcess::StartProcess(const QString &exe_path,
                                const QStringList &args)
{
    start(exe_path, args);
    if (!waitForStarted(300)) {
        qprint << "start process failed" << errorString() << "[" << exe_path
               << args << "]";
        kill();
        return false;
    }
    return true;
}

CPlayProcess::~CPlayProcess()
{
    QuitProcess();
}
