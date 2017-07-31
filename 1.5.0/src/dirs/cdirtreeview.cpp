#include "cdirtreeview.h"
#include "cdiritem.h"
#include "stable.h"

#include <qevent.h>
#include <QStandardItemModel>
#include "qapplication.h"
#include "qheaderview.h"

CDirTreeView::CDirTreeView(const DirDataPtr &data, QWidget *parent /*= 0*/)
    : QTreeView(parent),
      m_pitem_lastclick(NULL),
      m_data(data),
      m_is_first_load(true)
{
    connect(m_data.data(), &CDirModelDataHelper::SigSubOk, this,
            &CDirTreeView::SlotSubFolderDataReady);
}

void CDirTreeView::mousePressEvent(QMouseEvent *event)
{
    const QModelIndex clicked_index = indexAt(event->pos());
    if (clicked_index.isValid()) {
        const int item_identation
            = visualRect(clicked_index).x() - visualRect(rootIndex()).x();
        if (event->pos().x() < item_identation) {
            if (auto m = (QStandardItemModel *)model()) {
                if (auto item = (CDirItem *)m->itemFromIndex(clicked_index)) {
                    if (item->GetStatue() != CDirItem::Loading
                        && item->hasChildren()) {
                        TreeCtrlClicked(clicked_index);
                        return;
                    }
                }
            }
        }
    }
    return QTreeView::mousePressEvent(event);
}

QString CDirTreeView::GetSelectedItemPath()
{
    if (m_pitem_lastclick) {
        return m_pitem_lastclick->GetPath();
    }
    return {};
}

void CDirTreeView::TreeCtrlClicked(const QModelIndex &index)
{
    if (this->isExpanded(index)) {
        this->collapse(index);
    }
    else if (!m_model.isNull()) {
        if (auto item = (CDirItem *)m_model->itemFromIndex(index)) {
            if (item->GetStatue() == CDirItem::Loaded) {
                this->expand(index);
            }
            else if (item->GetStatue() == CDirItem::Unload) {
                item->setEnabled(false);
                if (auto horizontal
                    = (QStandardItem *)item->data().value<void *>()) {
                    horizontal->setEnabled(false);
                }
                m_data->ExpandSubFolder(item);
            }
        }
    }
}

void CDirTreeView::SetModel(const ModelPtr &model)
{
    m_model = model;
    setModel(m_model.data());
    connect(selectionModel(), &QItemSelectionModel::selectionChanged,
            [=](const QItemSelection &selected, const QItemSelection &) {
                if (!selected.isEmpty()) {
                    if (auto item_sub = (CDirItem *)model->itemFromIndex(
                            selected.indexes().first())) {
                        m_pitem_lastclick = item_sub;
                        emit SigSelectionChanged(m_pitem_lastclick->GetPath());
                    }
                }
            });
}

void CDirTreeView::SlotSubFolderDataReady(QStandardItem *item)
{
    if (auto i = (CDirItem *)item) {
        i->SetStatus(CDirItem::Loaded);
        i->setEnabled(true);
        if (this->isVisible()) {
            this->expand(i->index());
        }
    }
}
