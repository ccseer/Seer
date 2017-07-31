#include "cfilesystemhorizontalwnd.h"
#include "cdiritem.h"
#include "stable.h"
#include "ui_cfilesystemhorizontalwnd.h"

#include <QKeyEvent>
#include "qdatetime.h"
#include "qfileinfo.h"
#include "qfilesystemmodel.h"
#include "qitemselectionmodel.h"
#include "qlistview.h"
#include "qscrollbar.h"

CFileSystemHorizontalWnd::CFileSystemHorizontalWnd(const DirDataPtr &data,
                                                   QWidget *parent)
    : QWidget(parent),
      m_data(data),
      m_pitem_lastdir(NULL),
      ui(new Ui::CFileSystemHorizontalWnd)
{
    ui->setupUi(this);
    connect(m_data.data(), &CDirModelDataHelper::SigSubOk, this,
            &CFileSystemHorizontalWnd::SlotSubFolderDataReady);
}

CFileSystemHorizontalWnd::~CFileSystemHorizontalWnd()
{
    delete ui;
}

void CFileSystemHorizontalWnd::Clear()
{
    foreach (auto i,
             ui->scrollAreaWidgetContents->findChildren<QListView *>()) {
        delete i;
    }
}

QString CFileSystemHorizontalWnd::GetSelectedItemPath()
{
    foreach (auto i,
             ui->scrollAreaWidgetContents->findChildren<QListView *>()) {
        if (i->hasFocus()) {
            if (i->selectionModel()->selection().indexes().isEmpty()) {
                return {};
            }
            const auto index
                = i->selectionModel()->selection().indexes().first();
            if (index.isValid()) {
                if (auto item = (CDirItem *)m_model->itemFromIndex(index)) {
                    return item->GetPath();
                }
            }
            return {};
        }
    }
    return {};
}

void CFileSystemHorizontalWnd::SetModel(const ModelPtr &model)
{
    m_model = model;

    AppendListView(m_model->invisibleRootItem());
}

void CFileSystemHorizontalWnd::AppendListView(
    QStandardItem *item_parent_original)
{
    auto view = new QListView;
    view->setModel(m_model.data());
    view->setUniformItemSizes(true);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QListView::NoEditTriggers);
    view->setTextElideMode(Qt::ElideMiddle);
    view->setRootIndex(item_parent_original->index());
    view->installEventFilter(this);

    if (!item_parent_original->rowCount()) {
        view->setObjectName("EmptyFolderView");
    }
    else {
        view->setObjectName("CFileSystemHView");
    }

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            [=](const QItemSelection &selected,
                const QItemSelection & /*deselected*/) {
                ItemClicked(view, selected);
            });

    ui->Layout_content->addWidget(view);
    // move scrollbar
    if (this->isVisible()) {
        ui->Layout_content->activate();
        qApp->processEvents();
        ui->scrollArea->ensureWidgetVisible(view);
    }
}

void CFileSystemHorizontalWnd::SlotSubFolderDataReady(
    QStandardItem *item_parent_original)
{
    if (auto i = (CDirItem *)item_parent_original) {
        if (this->isVisible() && i == m_pitem_lastdir) {
            AppendListView(item_parent_original);
            m_pitem_lastdir = NULL;
        }
    }
    // loading 前要 setenable false, 导致 selected 状态消失
    foreach (auto i,
             ui->scrollAreaWidgetContents->findChildren<QListView *>()) {
        if (i->hasFocus()) {
            const auto active_index = i->currentIndex();
            if (active_index.isValid()) {
                if (item_parent_original
                    == m_model->itemFromIndex(active_index)) {
                    auto m = i->selectionModel();
                    m->clear();
                    m->select(active_index, QItemSelectionModel::ToggleCurrent);
                }
            }
            break;
        }
    }
}

void CFileSystemHorizontalWnd::ItemClicked(QListView *view,
                                           const QItemSelection &selected)
{
    // clear behind
    const auto index = ui->Layout_content->indexOf(view);
    if (index == -1 || selected.isEmpty()) {
        return;
    }

    foreach (auto i,
             ui->scrollAreaWidgetContents->findChildren<QListView *>()) {
        if (ui->Layout_content->indexOf(i) > index) {
            delete i;
        }
    }

    // add new view
    m_pitem_lastdir = NULL;
    if (CDirItem *item_dir_original
        = (CDirItem *)m_model->itemFromIndex(selected.indexes().first())) {
        emit SigSelectionChanged(GetSelectedItemPath());
        if (item_dir_original->hasChildren()) {
            if (item_dir_original->GetStatue() == CDirItem::Unload) {
                item_dir_original->setEnabled(false);
                m_pitem_lastdir = item_dir_original;
                // 等待接收信号到 SlotSubFolderDataReady
                m_data->ExpandSubFolder(m_pitem_lastdir);
            }
            else if (item_dir_original->GetStatue() == CDirItem::Loaded) {
                AppendListView(item_dir_original);
            }
        }
        else {
            if (QFileInfo(item_dir_original->GetPath()).isDir()) {
                AppendListView(item_dir_original);
            }
        }
    }
}

bool CFileSystemHorizontalWnd::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::KeyPress) {
        return false;
    }
    auto e         = (QKeyEvent *)event;
    const auto key = e->key();
    if (key != Qt::Key_Left && key != Qt::Key_Right && key != Qt::Key_Down
        && key != Qt::Key_Up) {
        return false;
    }
    QListView *view_cur = qobject_cast<QListView *>(watched);
    if (!view_cur) {
        return false;
    }

    const auto list_views
        = ui->scrollAreaWidgetContents->findChildren<QListView *>();
    if (!list_views.contains(view_cur)) {
        return true;
    }
    const auto vindex_cur = ui->Layout_content->indexOf(view_cur);
    if (key == Qt::Key_Down || key == Qt::Key_Up) {
        if (!m_model->rowCount()) {
            return true;
        }

        const auto root_index    = view_cur->rootIndex();
        QStandardItem *root_item = NULL;
        if (!root_index.isValid()) {
            if (vindex_cur != 0) {
                return true;
            }
            //为 第一个 时 取不到有效 index
            root_item = m_model->invisibleRootItem();
        }
        else {
            root_item = m_model.data()->itemFromIndex(root_index);
        }
        if (!root_item || !root_item->rowCount()) {
            return true;
        }
        const auto row_t = root_item->rowCount();

        auto m        = view_cur->selectionModel();
        auto row_next = -1;
        if (m->selection().indexes().isEmpty()) {
            // note: setenable 会导致 select 状态失效,
            // 失效时再去按键会导致从0开始
            //			row_next = (key == Qt::Key_Down ? 0 : row_t -
            // 1);
            return true;
        }
        else {
            const auto cur_row = m->selection().indexes().first().row();
            row_next = (key == Qt::Key_Down ? cur_row + 1 : cur_row - 1);
        }
        if (row_next >= row_t || row_next < 0) {
            return true;
        }
        auto result_index = root_item->child(row_next)->index();
        if (result_index.isValid()) {
            m->clear();
            m->select(result_index, m->SelectCurrent);
            view_cur->scrollTo(result_index);

            const auto p = GetSelectedItemPath();
            if (!p.isEmpty()) {
                emit SigSelectionChanged(p);
            }
        }
        return true;
    }

    // Key_Left    Key_Right
    const auto vindex_next = vindex_cur + (key == Qt::Key_Left ? -1 : 1);
    if (vindex_next < 0 || vindex_next >= list_views.size()) {
        return true;
    }

    if (auto wnd_next = ui->Layout_content->itemAt(vindex_next)) {
        if (auto view_next = (QListView *)wnd_next->widget()) {
            const auto cur_view_root_index    = view_next->rootIndex();
            QStandardItem *cur_view_root_item = NULL;
            if (!cur_view_root_index.isValid()) {
                if (vindex_next != 0) {
                    elprint;
                    return true;
                }
                //为 第一个 时 取不到有效 index
                cur_view_root_item = m_model->invisibleRootItem();
            }
            else {
                cur_view_root_item
                    = m_model.data()->itemFromIndex(cur_view_root_index);
            }

            if (!cur_view_root_item || !cur_view_root_item->rowCount()) {
                return true;
            }

            view_next->setFocus();
            auto m = view_next->selectionModel();
            QModelIndex index_select;
            if (m->selection().isEmpty()) {
                index_select = cur_view_root_item->child(0)->index();
                m->select(index_select, m->ToggleCurrent);
            }
            else {
                index_select = m->selection().indexes().first();
                m->select(index_select, m->SelectCurrent);
                const auto p = GetSelectedItemPath();
                if (!p.isEmpty()) {
                    emit SigSelectionChanged(p);
                }
            }
            ui->scrollArea->ensureWidgetVisible(view_next);
        }
    }

    return true;
}
