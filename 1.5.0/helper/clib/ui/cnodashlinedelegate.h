#ifndef CNODASHLINEDELEGATE_H
#define CNODASHLINEDELEGATE_H

#include <QStyledItemDelegate>

class CNoDashLineDelegate : public QStyledItemDelegate {
public:
    CNoDashLineDelegate(QObject *p = NULL) : QStyledItemDelegate(p)
    {
    }
    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
};

#endif  // CNODASHLINEDELEGATE_H
