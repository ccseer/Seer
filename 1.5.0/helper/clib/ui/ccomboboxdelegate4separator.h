#ifndef CCOMBOBOXDELEGATE4SEPARATOR_H
#define CCOMBOBOXDELEGATE4SEPARATOR_H

#include <QStyledItemDelegate>

class CComboBoxDelegate4Separator : public QStyledItemDelegate {
public:
    explicit CComboBoxDelegate4Separator(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {
    }

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
};
#endif  // CCOMBOBOXDELEGATE4SEPARATOR_H
