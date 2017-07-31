#include "cdirmaneger.h"
#include "cglobaldata.h"

#include <qfileinfo.h>
#include <QApplication>
#include <QFile>
#include <QTextCodec>

#ifdef Q_OS_WIN
#pragma execution_character_set("utf-8")
#endif

CDirManeger::CDirManeger() : CAbstractManager()
{
    connect(&m_process, &QProcess::readyReadStandardOutput, [=]() {
        const auto res = m_process.readAllStandardOutput();
        if (res.isEmpty()) {
            return;
        }
        QTextCodec *codec = QTextCodec::codecForName("System");
        m_pro_res.append(codec->toUnicode(res));
    });

    connect(&m_process, &CPlayProcess::SigFinishedNaturally, [=]() {
        Databox res_out;
        g_data->sub_type == S_Rar ? ParseRarLine(m_pro_res, res_out)
                                  : ParseZipLine(m_pro_res, res_out);
        if (res_out.isEmpty()) {
            SetErrMsg();
            emit SigPerformFinished();
            return;
        }

        Sorting(res_out);

        emit SigPerformFinished(QVariant::fromValue(res_out));
    });
}

CDirManeger::~CDirManeger()
{
    m_pro_res.clear();
    m_process.QuitProcess();
}

void CDirManeger::PerformerPrivate()
{
    const QString exe_path
        = (g_data->sub_type == S_Rar ? g_plugin_path_rar : g_plugin_path_7z);
    const QString exe = QFileInfo(exe_path).fileName();
    if (!QFile::exists(exe_path)) {
        SetErrMsg(exe + tr(" not found."));
        emit SigPerformFinished();
        return;
    }
    QStringList args;
    args << (g_data->sub_type == S_Rar ? "v" : "l");
    args << g_data->path;

    m_pro_res.clear();
    if (!m_process.StartProcess(exe_path, args)) {
        SetErrMsg(tr("Can not run ") + exe + ".");
        emit SigPerformFinished();
    }
}

void CDirManeger::Sorting(Databox &src)
{
    std::stable_sort(src.begin(), src.end(),
                     [](const ComData &a, const ComData &b) {
                         return a.filename.length() < b.filename.length();
                     });
}

void CDirManeger::ParseRarLine(const QString &arg, Databox &in)
{
    const QString str_mid
        = "--------------------------------------------------------------------"
          "-----------";
    auto src = arg.mid(arg.indexOf(str_mid) + str_mid.size());
    src.truncate(src.lastIndexOf(str_mid));

    bool is_file_name = true;
    QString file_name;
    foreach (auto i, src.split('\n')) {
        QString line = i.trimmed();
        if (!line.isEmpty()) {
            if (is_file_name) {
                file_name = line;
            }
            else {
                //当文件名中有空格时候,判断失效.   前面4个只要一个空格判断.
                //后面的为一个 . 暂假定数据完整.
                //      接着返回数据. 在表格显示;
                QStringList list = line.split(" ", QString::SkipEmptyParts);
                if (list.size() != 9) {
                    in.clear();
                    elprint;
                    return;
                }

                ComData data;
                data.filename = file_name;
                data.size     = list.at(0).trimmed().toUInt();
                data.packed   = list.at(1).trimmed().toUInt();
                data.ratio    = list.at(2).trimmed();
                data.time     = QDateTime(
                    QDate::fromString(list.at(3).trimmed(), "dd-MM-yy"),
                    QTime::fromString(list.at(4).trimmed(), "hh:mm"));
                data.is_dir = list.at(5).contains(".D.");

                in.append(data);
            }

            is_file_name = !is_file_name;
        }
    }
}

void CDirManeger::ParseZipLine(const QString &arg, Databox &in)
{
    const QString str_mid
        = "------------------- ----- ------------ ------------  "
          "------------------------";
    auto src = arg.mid(arg.indexOf(str_mid) + str_mid.size());
    src.truncate(src.lastIndexOf(str_mid));

    foreach (auto i, src.split('\n')) {
        QString line = i.trimmed();
        if (!line.isEmpty()) {
            QStringList list = ParseZipLineData(line);
            if (list.isEmpty()) {
                in.clear();
                elprint;
                return;
            }

            QString ratio = "0%";
            if (list.at(2).toInt() != 0) {
                ratio = QString().setNum(list.at(3).toUInt() * 1.00
                                             / list.at(2).toUInt() * 100,
                                         'f', 2)
                        + "%";
            }
            ComData data;
            data.filename = list.at(4);
            data.size     = list.at(2).toInt();
            data.packed   = list.at(3).toInt();
            data.ratio    = ratio;

            //"2013-04-09 16:19:58"
            data.time
                = QDateTime::fromString(list.first(), "yyyy-MM-dd hh:mm:ss");
            data.is_dir = list.at(1).startsWith("D");

            //构造临时变量会崩溃. 原因未知
            in.append(data);
        }
    }
}

QStringList CDirManeger::ParseZipLineData(const QString &src)
{
    QStringList res;

    //根据如下结构来解析; --> ("------------------- ----- ------------
    //------------  ------------------------");
    res << src.mid(0, QString("1-----------------2").length());
    res << src.mid(QString("1-----------------2 ").length(),
                   QString("3---4").length());
    res << src.mid(QString("1-----------------2 3---4 ").length(),
                   QString("5----------6").length());
    res << src.mid(QString("1-----------------2 3---4 5----------6 ").length(),
                   QString("7----------8").length());
    res << src.mid(
        QString("1-----------------2 3---4 5----------6 7----------8 ").length()
        + 1);

    return res;
}
