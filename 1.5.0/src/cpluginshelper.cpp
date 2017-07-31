#include "cpluginshelper.h"
#include "ccomponent.h"
#include "cfileproperty.h"
#include "cglobaldata.h"
#include "stable.h"
#include "types.h"
#include "utilities.h"

#include "qdir.h"
#include "qfileinfo.h"

CPluginsHelper::CPluginsHelper()
{
    connect(
        &m_process,
        (void (QProcess::*)(int, QProcess::ExitStatus)) & QProcess::finished,
        this, &CPluginsHelper::ProcessDone);

    // connect(&m_process,
    //	&QProcess::readyReadStandardOutput,
    //	this, [=]()
    //{
    //	qprint<<"plugin output:" << m_process.readAll();
    //});
}

CPluginsHelper::~CPluginsHelper()
{
    Clear();
}

void CPluginsHelper::Clear()
{
    m_process.QuitProcess();
    m_input.clear();
    m_output.clear();
}

void CPluginsHelper::ProcessDone(int /*code*/, QProcess::ExitStatus status)
{
    if (m_process.IsFinishedByKilled()) {
        return;
    }
    if (status == QProcess::CrashExit) {
        elprint << "QProcess::CrashExit" << m_process.errorString()
                << m_process.readAllStandardOutput();
        emit SigError();
        return;
    }

    if (m_input != g_data->path) {
        elprint << "plug-in & converted_file sync error";
        return;
    }

    const QFileInfo info_output(m_output);
    const QString name_no_suf = info_output.fileName();
    if (info_output.absoluteDir().exists()) {
        foreach (const auto &i,
                 info_output.absoluteDir().entryInfoList(
                     QStringList() << name_no_suf + ".*", QDir::Files)) {
            if (i.exists()) {
                m_last_convert = i.absoluteFilePath();
                emit SigSucceed(m_last_convert);
                return;
            }
        }
    }
    else {
        elprint;
    }
    emit SigError();
}

void CPluginsHelper::ClearLastPath()
{
    m_last_convert.clear();
    m_last_source.clear();
}

QString CPluginsHelper::CheckPlugins(const QString &file)
{
    const QString suf(QFileInfo(file).suffix());
    if (suf.isEmpty()) {
        return {};
    }
    foreach (const PluginData &i, g_data->plugins) {
        if (i.suffix.contains(suf, Qt::CaseInsensitive)) {
            return i.para;
        }
    }
    return {};
}

bool CPluginsHelper::IsFileFromPluginConverted(const QString &path)
{
    return path == m_last_convert;
}

bool CPluginsHelper::LoadPlugin(const QString &path)
{
    Clear();
    ClearLastPath();

    const auto plu_path = CheckPlugins(path);
    if (plu_path.isEmpty()) {
        return false;
    }
    // this is a plugins  -> Md5Ready
    m_process.setProgram(plu_path);
    m_last_source = path;
    return true;
}

void CPluginsHelper::Md5Ready(const QString &md5, const QString &target)
{
    // 正常加载时会还是会接收到信号
    // 此处判断是否是 非插件文件 加载
    if (m_last_source.isEmpty() || m_last_source != target) {
        return;
    }
    if (md5.isEmpty()) {
        elprint;
        emit SigError();
        return;
    }
    //拿到 md5 后开始检查是否已经转换完成
    //是则加载已经转换完的,没有则开进程转换
    const QString path_ouput(QDir::toNativeSeparators(GetAppTempPath() + md5));

    // static
    const static QDir dir(GetAppTempPath());
    if (dir.exists()) {
        foreach (const auto &i,
                 dir.entryInfoList(
                     QStringList() << QFileInfo(path_ouput).fileName() + ".*",
                     QDir::Files)) {
            m_last_convert = i.absoluteFilePath();
            emit SigSucceed(m_last_convert);
            return;
        }
    }

    QString exe_args = m_process.program();

    m_process.setProgram({});
    QStringList args;
    if (exe_args.contains(g_plugin_input_placeholder, Qt::CaseInsensitive)) {
        exe_args.replace(g_plugin_input_placeholder,
                         QDir::toNativeSeparators(target));
    }
    else {
        args << QDir::toNativeSeparators(target);
    }
    if (exe_args.contains(g_plugin_output_placeholder, Qt::CaseInsensitive)) {
        exe_args.replace(g_plugin_output_placeholder, path_ouput);
    }
    else {
        args << path_ouput;
    }

    m_input  = target;
    m_output = path_ouput;
    m_process.setWorkingDirectory(QFileInfo(SplitExePathAndExeArgs(exe_args))
                                      .absoluteDir()
                                      .absolutePath());
    args.isEmpty() ? m_process.start(exe_args)
                   : m_process.start(exe_args, args);

    if (!m_process.waitForStarted(1000)) {
        qprint << "Start process failed, reason:" << m_process.errorString()
               << endl
               << m_process.workingDirectory() << endl
               << m_process.program() << endl
               << m_process.arguments() << endl
               << endl;
        m_process.kill();
        emit SigError();
    }
}

QString CPluginsHelper::SplitExePathAndExeArgs(const QString &params)
{
    QStringList list = params.split(" ");
    if (list.size() == 0) {
        elprint;
        return {};
    }
    if (list.size() == 1)  //一个exe
    {
        elprint;
        return QFileInfo(params).absoluteDir().absolutePath();
    }

    QString exec_path;
    QFileInfo info;
    foreach (const auto &i, params) {
        if (i == QChar('\"')) {
            continue;
        }

        exec_path.append(i);
        info.setFile(exec_path);
        if (!info.exists() || info.isDir()) {
        }
        else {
            break;
            // qprint << "fffffffffffffffffffffff" <<
            // info.absoluteDir().absolutePath();
            // return info.absoluteDir().absolutePath();
        }
    }
    return exec_path;
}
