#include "chtmlmanager.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "utilities.h"

#include <QTextCodec>
#include "qapplication.h"
#include "qfile.h"
#include "qfileinfo.h"

CHtmlManager::CHtmlManager() : CAbstractManager()
{
    connect(&m_process, &QProcess::readyReadStandardOutput, this,
            [=]() { m_pro_res.append(m_process.readAllStandardOutput()); });
    connect(&m_process, &CPlayProcess::SigFinishedByKilled, this,
            [=] { emit SigPerformFinished(); });
    connect(&m_process, &CPlayProcess::SigFinishedNaturally, this, [=]() {
        if (GetErrMsg().isEmpty() || !m_pro_res.isEmpty()) {
#ifdef DECODE
            const QByteArray charset
                = C::Core::Funcs::GetCharSetName(m_pro_res);
            if (!charset.isEmpty()) {
                if (QTextCodec *codec = QTextCodec::codecForName(charset)) {
                    emit SigPerformFinished(
                        WriteMD2File(codec->toUnicode(m_pro_res)));
                    return;
                }
            }
#endif
            emit SigPerformFinished(WriteMD2File(QString::fromUtf8(m_pro_res)));
            return;
        }

        SetErrMsg(tr("Parse file error."));
        emit SigPerformFinished();
    });
}

QString CHtmlManager::GetConstantHtml4Markdown(const QString &input)
{
    auto css_path = g_pini->GetIni(Ini::markdown_css_path_str).toString();
    if (!QFile::exists(css_path)) {
        css_path = g_pstyle->GetHtmlMarkdownPath();
    }
    QFile f(css_path);
    f.open(f.ReadOnly);
    const QString css = f.readAll();
    f.close();

    return "<!DOCTYPE html>\n"
         "<html  lang=\"en\"><head>\n"
         "<meta charset=\"UTF-8\"/>\n"
         "<style type=\"text/css\">\n" +
         css + "</style>"
               "</head><body>\n" +
         input + "\n</body></html>\n";
}

CHtmlManager::~CHtmlManager()
{
    m_process.QuitProcess();
}

void CHtmlManager::LoadMarkdown()
{
    ExecProcess(g_plugin_dir + "md.exe", g_data->path);
}

void CHtmlManager::LoadWeb()
{
    emit SigPerformFinished(g_data->path);
}

void CHtmlManager::ExecProcess(const QString &exe_path,
                               const QString &path,
                               const QStringList &args)
{
    const auto name = QFileInfo(exe_path).fileName();
    if (!QFile::exists(exe_path)) {
        SetErrMsg(exe_path + tr(" not found."));
        emit SigPerformFinished();
        return;
    }
    m_pro_res.clear();

    QStringList parameters = QStringList{} << path;
    if (!args.isEmpty()) {
        parameters << args;
    }

    if (!m_process.StartProcess(exe_path, parameters)) {
        SetErrMsg(tr("Can not run ") + name + ".");
        emit SigPerformFinished();
    }
}

void CHtmlManager::PerformerPrivate()
{
    switch (g_data->sub_type) {
        case S_Mrakdown:
            LoadMarkdown();
            break;
        case S_Web:
            LoadWeb();
            break;
        default:
            elprint;
            break;
    }
}

QString CHtmlManager::WriteMD2File(const QString &res)
{
    auto c             = GetConstantHtml4Markdown(res);
    const auto md_html = GetAppTempPath() + "md.html";
    QFile f(md_html);
    if (!f.open(f.Truncate | f.WriteOnly)) {
        return {};
    }
    if (-1 == f.write(c.toUtf8())) {
        return {};
    }
    f.close();
    return md_html;
}
