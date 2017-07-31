#include "ctableviewwithcopykey.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cnodashlinedelegate.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <qevent.h>
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QHeaderView>
#include <QItemSelectionModel>
#include "qscrollbar.h"
#include "qstandarditemmodel.h"

QString CTableViewWithCopyKey::s_selected_view_key;

CTableViewWithCopyKey::CTableViewWithCopyKey(QWidget *parent /*= 0*/)
    : QTreeView(parent)
{
    setItemDelegate(new CNoDashLineDelegate(this));

    setTextElideMode(Qt::ElideMiddle);
    setEditTriggers(QTreeView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setExpandsOnDoubleClick(false);
    setRootIsDecorated(false);
    setMouseTracking(true);
    setAnimated(true);

    connect(this, &QTreeView::clicked, this, [=](const QModelIndex &index) {
        if (!index.isValid()) {
            return;
        }
        if (QAbstractItemModel *m = model()) {
            auto index_details = m->index(index.row(), 0, index.parent());
            if (index_details.isValid() && m->hasChildren(index_details)) {
                this->isExpanded(index_details) ? collapse(index_details)
                                                : expand(index_details);
            }
        }
    });
    connect(this, &QTreeView::expanded, this,
            [=](const QModelIndex & /*index*/) {
                g_pini->SetIni(Ini::expand_details_property_b, true);
            });
    connect(this, &QTreeView::collapsed, this,
            [=](const QModelIndex & /*index*/) {
                g_pini->SetIni(Ini::expand_details_property_b, false);
            });
}

void CTableViewWithCopyKey::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header()->setVisible(false);
    // horizontalHeader()->setSectionResizeMode(0,
    // QHeaderView::ResizeToContents);
    // horizontalHeader()->setStretchLastSection(true);
    // horizontalHeader()->setVisible(false);
    // verticalHeader()->setVisible(false);
    connect(
        model, &QAbstractItemModel::rowsInserted, this,
        [=](const QModelIndex &parent, int /*first*/, int /*last*/) {
            if (parent.isValid()
                && parent.data().toString().trimmed().toUtf8().data()
                       == qApp->translate("CFileProperty", g_pp_details)) {
                bool expanded
                    = g_pini->GetIni(Ini::expand_details_property_b).toBool();
                if (expanded != isExpanded(parent)) {
                    if (expanded) {
                        expand(parent);
                    }
                    else {
                        collapse(parent);
                    }
                }
            }
            UpdateCurRow();
        });
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [=]() { SaveCurRow(); });
}

void CTableViewWithCopyKey::keyPressEvent(QKeyEvent *e)
{
    const auto indexes = selectionModel()->selection().indexes();
    if (indexes.isEmpty()) {
        return QTreeView::keyReleaseEvent(e);
    }

    if (e->matches(QKeySequence::Copy)) {
        QString res = indexes.last().data().toString();
        qApp->clipboard()->setText(res);
    }
    else if (e->modifiers().testFlag(Qt::ShiftModifier)
             && e->modifiers().testFlag(Qt::ControlModifier)
             && e->key() == Qt::Key_C) {
        // column = 0
        const auto index = indexes.first();
        const QString path
            = model()
                  ->index(index.row(), model()->columnCount() - 1,
                          index.parent())
                  .data()
                  .toString();
        const QString name
            = model()
                  ->index(0, model()->columnCount() - 1, index.parent())
                  .data()
                  .toString();
        // path
        if (index.row() == 2) {
            qApp->clipboard()->setText(QDir(path).absoluteFilePath(name));
        }
        // name
        else if (index.row() == 0) {
            qApp->clipboard()->setText(
                QDir::toNativeSeparators(QDir(path).absoluteFilePath(name)));
        }
    }
}

void CTableViewWithCopyKey::mouseMoveEvent(QMouseEvent *e)
{
    if (auto m = (QStandardItemModel *)model()) {
        QModelIndex index = indexAt(e->pos());
        if (index.isValid()
            && m->index(index.row(), 0, index.parent())
                       .data()
                       .toString()
                       .trimmed()
                       .toUtf8()
                       .data()
                   == qApp->translate("CFileProperty", g_pp_details)) {
            setCursor(Qt::PointingHandCursor);
        }
        else {
            setCursor(Qt::ArrowCursor);
        }
    }
    QTreeView::mouseMoveEvent(e);
}

void CTableViewWithCopyKey::UpdateCurRow()
{
    if (auto m = (QStandardItemModel *)model()) {
        QList<QStandardItem *> res;
        C::Ui::TreeModel::GetAllSubItems(m->invisibleRootItem(), res,
                                         Qt::DisplayRole, s_selected_view_key);
        foreach (auto i, res) {
            auto i_row   = m->indexFromItem(i);
            auto index_d = m->index(i_row.row(), 1, i_row.parent());
            selectionModel()->select(
                index_d,
                QItemSelectionModel::Rows | QItemSelectionModel::ToggleCurrent);
            scrollTo(index_d);
            break;
        }
    }
}

void CTableViewWithCopyKey::SaveCurRow()
{
    const QModelIndexList indexes = selectionModel()->selectedRows(0);
    if (!indexes.isEmpty()) {
        const auto index    = indexes.first();
        s_selected_view_key = index.data().toString();
    }
}
