#include "calignedmodel.h"
#include "stable.h"

CAlignedModel::CAlignedModel(QObject *parent) : QStandardItemModel(parent)
{
}

QVariant CAlignedModel::data(const QModelIndex &index, int role) const
{
    QVariant val = QStandardItemModel::data(index, role);
    if (Qt::TextAlignmentRole == role
        && m_align_column.contains(index.column())) {
        val = int(Qt::AlignRight | Qt::AlignVCenter);
    }
    return val;
}
