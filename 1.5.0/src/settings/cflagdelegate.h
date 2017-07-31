#ifndef CPROPERTYSTYLEDELEGATE_H
#define CPROPERTYSTYLEDELEGATE_H

#include <QApplication>
#include <QStyledItemDelegate>

const int g_orig_icon_role = Qt::UserRole + 20;
const int g_cur_role       = Qt::UserRole + 10;

class CFlagDelegate : public QStyledItemDelegate {
public:
    CFlagDelegate(QObject *parent = Q_NULLPTR);

    void UpdateDpi(qreal r);
    qreal GetFlagBaseSz();

    // QAbstractItemDelegate interface
protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    QPixmap m_pix_star;
    qreal m_ratio;
};

#endif  // CPROPERTYSTYLEDELEGATE_H
