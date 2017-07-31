#include "cfileproperty.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cnodashlinedelegate.h"
#include "ctableviewwithcopykey.h"
#include "cthememanager.h"
#include "global_headers.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <QLocale>
#include "qaction.h"
#include "qcoreevent.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include <QScrollBar>

#define g_datetime_format QLocale::system().dateTimeFormat(QLocale::ShortFormat)

#define g_pp_name QT_TRANSLATE_NOOP("CFileProperty", "Name")
#define g_pp_size QT_TRANSLATE_NOOP("CFileProperty", "Size")
#define g_pp_path QT_TRANSLATE_NOOP("CFileProperty", "Location")
#define g_pp_o_path QT_TRANSLATE_NOOP("CFileProperty", "Original location")
#define g_pp_type QT_TRANSLATE_NOOP("CFileProperty", "Type")
#define g_pp_exe_file QT_TRANSLATE_NOOP("CFileProperty", "Opens with")
#define g_pp_create_time QT_TRANSLATE_NOOP("CFileProperty", "Created")
#define g_pp_mod_time QT_TRANSLATE_NOOP("CFileProperty", "Modified")
#define g_pp_read_time QT_TRANSLATE_NOOP("CFileProperty", "Accessed")
#define g_pp_guess_extension QT_TRANSLATE_NOOP("CFileProperty", "Extension")

CFileProperty::CFileProperty(QObject *parent)
    : QObject(parent),
      m_pmodel(new QStandardItemModel(this)),
      m_pview(new CTableViewWithCopyKey)
{
    m_pmodel->setColumnCount(2);
    m_pview->setObjectName("FilePropertyView");
    m_pview->verticalScrollBar()->setObjectName("sub");
    m_pview->setModel(m_pmodel);
    m_pview->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                            | Qt::SubWindow);

    m_paction = new QAction(m_pview);
    connect(m_paction, &QAction::triggered, this, &CFileProperty::Hide);

    SlotUpdateShortcut4ViewWnd();
    m_pview->addAction(m_paction);
    m_pview->installEventFilter(this);
    connect(&m_process, (void (QProcess::*)(int)) & QProcess::finished, this,
            &CFileProperty::SlotProcessFinished);
}

CFileProperty::~CFileProperty()
{
    cfree(m_pview);
    m_process.QuitProcess();
}

void CFileProperty::SlotProcessFinished(int /*code*/)
{
    if (m_process.IsFinishedByKilled()) {
        return;
    }
    const auto path_json = m_process.arguments().last();
    QFile file(path_json);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }
    const QJsonDocument json_doc(QJsonDocument::fromJson(file.readAll()));
    file.close();
    file.remove();
    if (json_doc.isNull()) {
        return;
    }
    const auto json_arr = json_doc.array();
    if (json_arr.isEmpty()) {
        return;
    }
    FilePropertyData res_data;
    QString md5;
    foreach (const QVariant &i, json_arr.toVariantList()) {
        const auto map = i.toMap();
        foreach (const auto &j, map.keys()) {
            FileProperty t;
            t.key   = j;
            t.value = map[j].toString();
            if (0 == t.key.compare("md5", Qt::CaseInsensitive)) {
                md5 = t.value;
            }
            res_data.append(t);
        }
    }
    AppendFileProperty(res_data);

    const auto args = m_process.arguments();
    emit SigPropertyReady(md5, args.value(args.size() - 2));
}

void CFileProperty::Clear()
{
    m_process.QuitProcess();
    m_pmodel->setRowCount(0);
    m_path.clear();
}

void CFileProperty::Load(const QString &filepath)
{
    g_pfile_property->Clear();
    m_path = filepath;

    const QFileInfo info(filepath);
    auto item_file_name = new QStandardItem(info.fileName());
    item_file_name->setIcon(GetFileIcon(filepath));
    AppenItemsRow(tr(g_pp_name), item_file_name);
    if (info.isFile()) {
        AppenItemsRow(tr(g_pp_size),
                      C::Core::Funcs::ConvertSizeNum2String(info.size()));
    }
    AppenItemsRow(tr(g_pp_path), info.absoluteDir().absolutePath());
    if (info.isSymLink()) {
        AppenItemsRow(tr(g_pp_o_path), info.symLinkTarget());
    }

    CoInitialize(NULL);
    SHFILEINFO sfi = {0};
    SHGetFileInfo((const wchar_t *)filepath.utf16(), 0, &sfi, sizeof(sfi),
                  SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);
    const QString file_type = QString::fromWCharArray(sfi.szTypeName);
    CoUninitialize();
    AppenItemsRow(tr(g_pp_type), file_type);

    if (info.isFile() && info.suffix().toLower() != "exe") {
        if (info.suffix().isEmpty() && !g_data->suffix.isEmpty()) {
            AppenItemsRow(tr(g_pp_guess_extension), g_data->suffix);
        }
        else {
            const auto exec = GetExecFilePath(filepath);
            if (!exec.isEmpty()) {
                auto item = new QStandardItem(QFileInfo(exec).fileName());
                item->setIcon(GetFileIcon(exec));
                AppenItemsRow(tr(g_pp_exe_file), item);
            }
        }
    }

    AppenItemsRow(tr(g_pp_create_time),
                  info.created().toString(g_datetime_format));
    AppenItemsRow(tr(g_pp_mod_time),
                  info.lastModified().toString(g_datetime_format));
    AppenItemsRow(tr(g_pp_read_time),
                  info.lastRead().toString(g_datetime_format));

    if (info.isFile() && (info.suffix().toLower() == "exe"
                          || info.suffix().toLower() == "dll")) {
        QMap<QString, QString> res;
        GetExeFilePropertyDetails(filepath, res);

        if (!res.isEmpty()) {
            auto item_parent
                = AppenItemsRow(tr(g_pp_details), new QStandardItem(),
                                m_pmodel->invisibleRootItem())
                      .first();
            auto it_res = res.constBegin();
            while (it_res != res.constEnd()) {
                AppenItemsRow(it_res.key(), new QStandardItem(it_res.value()),
                              item_parent);
                ++it_res;
            }
        }
    }

    QStringList args;
    QFileInfo(filepath).isDir() ? args << QString::number(IT_Dir_Size)
                                : args << QString::number(IT_MD5_SHA1);
    args << filepath
         << C::Core::DirFile::GetUniqueFilePath(GetFilePropertyTempPath())
                + ".json";
    m_process.StartProcess(g_name_helper, args);
}

bool CFileProperty::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_pview) {
        switch (e->type()) {
            case QEvent::WindowDeactivate:
                Hide();
                break;
            case QEvent::Show:
                emit SigViewShow(true);
                break;
            case QEvent::Hide:
                emit SigViewShow(false);
                break;
        }
    }
    return false;
}

void CFileProperty::Hide()
{
    m_pview->setVisible(false);
}

void CFileProperty::AppendFileProperty(const FilePropertyData &data)
{
    foreach (const auto &i, data) {
        auto item_val = new QStandardItem(i.value);
        item_val->setBackground(g_pstyle->GetInfoDynamicRowBgColor());
        AppenItemsRow(i.key, item_val)
            .first()
            ->setBackground(g_pstyle->GetInfoDynamicRowBgColor());
    }
}

void CFileProperty::Show(const QRect &parent_rt, qreal ratio)
{
    const auto max_row = qMax(m_pmodel->rowCount(), 10);
    const auto sz
        = QSize(GetMinWndSz(ratio).width(),
                m_pview->visualRect(m_pmodel->index(0, 0)).height() * max_row);
    auto rt = QRect(m_pview->rect().topLeft(), sz);
    rt.moveCenter(parent_rt.center());
    m_pview->setGeometry(rt);
    m_pview->setVisible(true);
    m_pview->setWindowOpacity(0.96);
    C::Ui::Funcs::MoveWndInsideDesktop(m_pview);
    auto f = qApp->font();
    f.setPixelSize(ratio * 12);
    m_pview->setFont(f);
}

QList<QStandardItem *> CFileProperty::AppenItemsRow(const QString &key,
                                                    const QString &val)
{
    return AppenItemsRow(key, new QStandardItem(val));
}

QList<QStandardItem *> CFileProperty::AppenItemsRow(const QString &key,
                                                    QStandardItem *item_val)
{
    return AppenItemsRow(key, item_val, m_pmodel->invisibleRootItem());
}

QList<QStandardItem *> CFileProperty::AppenItemsRow(const QString &key,
                                                    QStandardItem *item_val,
                                                    QStandardItem *item_parent)
{
#define g_spacer_front "    "
#define g_spacer_back "      "
    auto item = new QStandardItem(g_spacer_front + key + g_spacer_back);
    QList<QStandardItem *> list{item, item_val};
    item_parent->appendRow(list);
    return list;
}

void CFileProperty::SlotUpdateShortcut4ViewWnd()
{
    m_paction->setShortcut(g_pini->GetIni(Ini::key_property_str).toString());
}
