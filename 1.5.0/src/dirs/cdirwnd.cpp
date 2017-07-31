#include "cdirwnd.h"
#include "calignedmodel.h"
#include "ccomponent.h"
#include "cdiritem.h"
#include "cdirmaneger.h"
#include "cdirtreeview.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "utilities.h"

#include <QApplication>
#include <QClipboard>
#include <QStandardItem>
#include <QStandardItemModel>
#include "qaction.h"
#include "qevent.h"
#include "qheaderview.h"

#include <functional>

#define g_time_format \
    QString(QLocale::system().dateFormat(QLocale::ShortFormat) + " hh:mm")

CDirWnd::CDirWnd(QWidget *parent)
    : CAbstractWnd(ManagerPtr(new CDirManeger), parent)
{
    qRegisterMetaType<ModelPtr>("ModelPtr");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    InitFolder();
    InitCompression();

    // container
    m_pwnd_stack = new QStackedWidget;
    m_pwnd_stack->addWidget(m_pview_folder_column);
    m_pwnd_stack->addWidget(m_pview_folder_tree);
    m_pwnd_stack->addWidget(m_pview_com_tree);
    m_pwnd_stack->addWidget(m_pview_com_column);
    SetWnd(m_pwnd_stack);

    QAction *a = new QAction(this);
    a->setShortcut(QKeySequence::Copy);
    connect(a, &QAction::triggered, this, &CDirWnd::Copy);
    this->addAction(a);
}

void CDirWnd::InitTreeView(QTreeView *view)
{
    view->setAnimated(true);
    view->setUniformRowHeights(true);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QTreeView::NoEditTriggers);
    view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view->header()->setMinimumSectionSize(
        view->header()->fontMetrics().width('M') * 10);
    view->header()->setSectionsClickable(false);
}

void CDirWnd::InitFolder()
{
    //写成初始化列表会崩溃. 原因未知  ????
    m_folder_data         = DirDataPtr(new CDirModelDataHelper);
    m_pview_folder_tree   = new CDirTreeView(m_folder_data);
    m_pview_folder_column = new CFileSystemHorizontalWnd(m_folder_data);

    connect(m_folder_data.data(), &CDirModelDataHelper::SigErr, this,
            &CDirWnd::SigErr);
    connect(m_folder_data.data(), &CDirModelDataHelper::SigOk, this,
            [=](const ModelPtr &ptr) {
                m_pview_folder_tree->SetModel(ptr);
                m_pview_folder_column->SetModel(ptr);
                InitTreeView(m_pview_folder_tree);

                emit SigLoaded();
            });
}

void CDirWnd::InitCompression()
{
    m_com_data         = DirDataPtr(new CDirModelDataHelper);
    m_pview_com_tree   = new QTreeView;
    m_pview_com_column = new CFileSystemHorizontalWnd(m_com_data);

    InitTreeView(m_pview_com_tree);
}

QString CDirWnd::GetCurIndexPath4Folder()
{
    QString path_selected
        = (m_pwnd_stack->currentWidget() == m_pview_folder_tree
               ? m_pview_folder_tree->GetSelectedItemPath()
               : m_pview_folder_column->GetSelectedItemPath());
    return path_selected;
}

void CDirWnd::SetBtnsEnable4Folder(bool e)
{
    m_pbtn_open->defaultAction()->setEnabled(e);
    m_pbtn_rev->defaultAction()->setEnabled(e);
    m_pbtn_eye->defaultAction()->setEnabled(e);
}

void CDirWnd::UpdateOpenByDefApp(const QString &p)
{
    const auto exec = GetExecFilePath(p);
    QIcon icon      = GetFileIcon(exec);
    m_pbtn_open->defaultAction()->setIcon(icon);
}

void CDirWnd::Copy()
{
    if (g_data->sub_type == S_Folder) {
        if (m_pbtn_eye->defaultAction()->isEnabled()) {
            QString str = GetCurIndexPath4Folder();
            if (!str.isEmpty()) {
                CopyFile2Clipboard(str);
            }
        }
    }
    else {
        if (m_pview_com_tree->isVisible()) {
            auto index = m_pview_com_tree->currentIndex();
            if (index.isValid()) {
                qApp->clipboard()->setText(index.data().toString());
            }
        }
        else {
            auto t = m_pview_com_column->GetSelectedItemPath();
            if (!t.isEmpty()) {
                qApp->clipboard()->setText(t);
            }
        }
    }
}

CDirWnd::~CDirWnd()
{
    m_folder_data->StopThreads();
}

void CDirWnd::InitControlWnd(QHBoxLayout *lay)
{
    // add view style btn
    {
        m_pbtn_folder_column = GetBottomBtn(
            fa::ctrl_folder_column, tr("Column View"),
            g_pini->GetIni(Ini::key_folder_column_view_str).toString());
        m_pbtn_folder_column->defaultAction()->setCheckable(true);
        m_pbtn_folder_tree = GetBottomBtn(
            fa::ctrl_folder_tree, tr("Tree View"),
            g_pini->GetIni(Ini::key_folder_tree_view_str).toString());
        m_pbtn_folder_tree->defaultAction()->setCheckable(true);
        const bool is_folder_treeview
            = g_pini->GetIni(Ini::is_folder_treeview_b).toBool();
        m_pbtn_folder_tree->defaultAction()->setChecked(is_folder_treeview);
        m_pbtn_folder_column->defaultAction()->setChecked(!is_folder_treeview);
        lay->addWidget(m_pbtn_folder_tree);
        lay->addWidget(m_pbtn_folder_column);
        // Exclusive
        QActionGroup *grp = new QActionGroup(this);
        grp->setExclusive(true);
        grp->addAction(m_pbtn_folder_tree->defaultAction());
        grp->addAction(m_pbtn_folder_column->defaultAction());
    }
    lay->addStretch();

    // S_Folder only
    if (g_data->sub_type == S_Folder) {
        m_pbtn_open
            = GetBottomBtn(fa::ctrl_open_file,
                           tr("Open Selected Item With Default Application"),
                           g_pini->GetIni(Ini::key_folder_open_str).toString());
        lay->addWidget(m_pbtn_open);
        this->addAction(m_pbtn_open->defaultAction());
        m_pbtn_rev = GetBottomBtn(
            fa::ctrl_reveal_file, tr("Display Selected Item In Explorer"),
            g_pini->GetIni(Ini::key_folder_reveal_str).toString());
        lay->addWidget(m_pbtn_rev);
        this->addAction(m_pbtn_rev->defaultAction());
        m_pbtn_eye = GetBottomBtn(
            fa::ctrl_seer_file, tr("Open Selected Item With Seer"),
            g_pini->GetIni(Ini::key_folder_open_seer_str).toString());
        lay->addWidget(m_pbtn_eye);
        this->addAction(m_pbtn_eye->defaultAction());

        //  connection :folder open btns
        connect(m_pbtn_open->defaultAction(), &QAction::triggered, this, [=]() {
            const auto str = GetCurIndexPath4Folder();
            if (!str.isEmpty()) {
                emit SigOpenByDefault(str);
            }
        });
        connect(m_pbtn_rev->defaultAction(), &QAction::triggered, this, [=]() {
            const auto str = GetCurIndexPath4Folder();
            if (!str.isEmpty()) {
                emit SigRevealInExplorer(str);
            }
        });
        connect(m_pbtn_eye->defaultAction(), &QAction::triggered, this, [=]() {
            const auto str = GetCurIndexPath4Folder();
            if (!str.isEmpty()) {
                emit SigLoadNewPath(str);
            }
        });

        // view
        connect(m_pview_folder_column,
                &CFileSystemHorizontalWnd::SigSelectionChanged, this,
                [=](const QString &selected) {
                    SetBtnsEnable4Folder(!selected.isEmpty());
                    UpdateOpenByDefApp(selected);
                });

        connect(m_pview_folder_tree, &CDirTreeView::SigSelectionChanged, this,
                [=](const QString &selected) {
                    SetBtnsEnable4Folder(!selected.isEmpty());
                    UpdateOpenByDefApp(selected);
                });

        // connection: view style btn
        connect(
            m_pbtn_folder_tree->defaultAction(), &QAction::triggered, this,
            [=](bool checked) {
                if (checked) {
                    m_pwnd_stack->setCurrentWidget(m_pview_folder_tree);
                    g_pini->SetIni(Ini::is_folder_treeview_b, true);
                    SetBtnsEnable4Folder(!GetCurIndexPath4Folder().isEmpty());
                }
            });
        connect(
            m_pbtn_folder_column->defaultAction(), &QAction::triggered, this,
            [=](bool checked) {
                if (checked) {
                    m_pwnd_stack->setCurrentWidget(m_pview_folder_column);
                    g_pini->SetIni(Ini::is_folder_treeview_b, false);
                    SetBtnsEnable4Folder(!GetCurIndexPath4Folder().isEmpty());
                }
            });
    }
    // compression
    else {
        // connection: view style btn
        connect(m_pbtn_folder_tree->defaultAction(), &QAction::triggered, this,
                [=](bool checked) {
                    if (checked) {
                        m_pwnd_stack->setCurrentWidget(m_pview_com_tree);
                        g_pini->SetIni(Ini::is_folder_treeview_b, true);
                    }
                });
        connect(m_pbtn_folder_column->defaultAction(), &QAction::triggered,
                this, [=](bool checked) {
                    if (checked) {
                        m_pwnd_stack->setCurrentWidget(m_pview_com_column);
                        g_pini->SetIni(Ini::is_folder_treeview_b, false);
                    }
                });
    }
}

void CDirWnd::LoadHelper()
{
    const auto is_column = m_pbtn_folder_column->defaultAction()->isChecked();
    if (S_Folder == g_data->sub_type) {
        is_column ? m_pbtn_folder_column->defaultAction()->trigger()
                  : m_pbtn_folder_tree->defaultAction()->trigger();

        //等待线程结束 -> InitFolder
        m_folder_data->SetPath(g_data->path);
    }
    else {
        is_column ? m_pbtn_folder_column->defaultAction()->trigger()
                  : m_pbtn_folder_tree->defaultAction()->trigger();
        // goto PerformFinished()
        m_manager->PerformerPrivate();
    }
}

// only works 4 compression
void CDirWnd::PerformFinished(const QVariant &v)
{
    const Databox res_out = v.value<Databox>();
    if (res_out.isEmpty()) {
        emit SigErr();
        return;
    }

    // need a sorted data, sorting by file name length. ascending sort
    QList<ComData> items_top_datas;
    QList<CDirItem *> items_top;
    {
        QHash<QString, CDirItem *> items_all;

        std::function<void(const ComData &)> lam = [&](const ComData &data) {
            const QString filename = data.filename;
            // rules --  windows : file name cant  have a  '\' character
            // file
            if (!filename.contains('\\')) {
                auto item = new CDirItem(filename, filename);
                item->SetStatus(CDirItem::Loaded);
                items_all.insert(filename, item);
                items_top.append(item);
                items_top_datas.append(data);
            }
            // dir
            else {
                const auto dir_parent
                    = filename.left(filename.lastIndexOf('\\'));
                if (CDirItem *item_parent = items_all.value(dir_parent, NULL)) {
                    auto item = new CDirItem(
                        QString(filename).remove(dir_parent + "\\"));
                    item->SetPath(item->text());
                    item->SetStatus(CDirItem::Loaded);
                    item_parent->appendRow(
                        QList<QStandardItem *>()
                        << item
                        << new QStandardItem(
                               data.is_dir
                                   ? ""
                                   : C::Core::Funcs::ConvertSizeNum2String(
                                         data.size))
                        << new QStandardItem(
                               data.is_dir
                                   ? ""
                                   : C::Core::Funcs::ConvertSizeNum2String(
                                         data.packed))
                        << new QStandardItem(data.is_dir ? "" : data.ratio)
                        << new QStandardItem(
                               data.time.toString(g_time_format)));

                    items_all.insert(filename, item);
                }
                else {
                    // sometimes zip.exe loses the dir data .
                    // elprint << dir_parent << "lose the dir";
                    ComData data_t{dir_parent, 0, 0, 0, data.time, true};
                    lam(data_t);
                }
            }
        };

        foreach (const auto &data, res_out) {
            lam(data);
        }
    }

    auto model_com = ModelPtr(new CAlignedModel);
    {
        model_com->setHorizontalHeaderLabels(QStringList() << tr("Filename")
                                                           << tr("Size")    // 1
                                                           << tr("Packed")  // 2
                                                           << tr("Ratio")   // 3
                                                           << tr("Time"));
        model_com->setColumnCount(5);
        AlignColumns hash;
        hash.insert(1, 1);
        hash.insert(2, 2);
        hash.insert(3, 3);
        model_com->SetAlignColumn(hash);
    }
    m_pview_com_tree->setModel(model_com.data());
    for (auto i = 0; i < items_top.count(); ++i) {
        const auto &item_data = items_top_datas.at(i);

        model_com->appendRow(
            QList<QStandardItem *>()
            << items_top.at(i)
            << new CDirItem(
                   item_data.is_dir
                       ? ""
                       : C::Core::Funcs::ConvertSizeNum2String(item_data.size))
            << new CDirItem(item_data.is_dir
                                ? ""
                                : C::Core::Funcs::ConvertSizeNum2String(
                                      item_data.packed))
            << new CDirItem(item_data.is_dir ? "" : item_data.ratio)
            << new CDirItem(item_data.time.toString(g_time_format)));
    }

    //压缩包 ->  如果只有一个 dir 在最外层 -> 自动展开
    if (model_com->rowCount() == 1) {
        m_pview_com_tree->expand(model_com->index(0, 0));
    }

    //    m_folder_data_cc->SetModel(m_model_com);
    m_pview_com_column->SetModel(model_com);

    emit SigLoaded();
}

QSize CDirWnd::GetSize()
{
    auto val = 9 * 80 * m_dpi->ratio();
    if (g_data->sub_type == S_Folder) {
        val = 9 * 100 * m_dpi->ratio();
    }
    return QSizeF(val, val * 3 / 4).toSize();
}
