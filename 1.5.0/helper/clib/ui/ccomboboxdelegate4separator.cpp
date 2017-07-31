#include "ccomboboxdelegate4separator.h"
#include <QPainter>

void CComboBoxDelegate4Separator::paint(QPainter *painter,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    if (index.data(Qt::AccessibleDescriptionRole).toString()
        == QLatin1String("separator")) {
        painter->setPen(Qt::gray);
        painter->drawLine(option.rect.left(), option.rect.center().y(),
                          option.rect.right(), option.rect.center().y());
    }
    else
        QStyledItemDelegate::paint(painter, option, index);
}

QSize CComboBoxDelegate4Separator::sizeHint(const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QString type = index.data(Qt::AccessibleDescriptionRole).toString();
    if (type == QLatin1String("separator"))
        return QSize(0, 2);
    return QStyledItemDelegate::sizeHint(option, index);
}
