#ifndef CGRAPHICSPIXMAPITEM_H
#define CGRAPHICSPIXMAPITEM_H

#include <QGraphicsPixmapItem>

class CGraphicsPixmapItemWithBg : public QGraphicsPixmapItem {
public:
    explicit CGraphicsPixmapItemWithBg(const QPixmap &pixmap,
                                       QGraphicsItem *parent = 0);

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    QImage m_transparent;
};

#endif  // CGRAPHICSPIXMAPITEM_H
