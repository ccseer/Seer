#include "cgraphicspixmapitemwithbg.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

CGraphicsPixmapItemWithBg::CGraphicsPixmapItemWithBg(const QPixmap &pixmap,
                                                     QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    m_transparent.load("://bg_transparent.png");
}

void CGraphicsPixmapItemWithBg::paint(QPainter *painter,
                                      const QStyleOptionGraphicsItem *option,
                                      QWidget *widget)
{
    painter->save();

    painter->setOpacity(0.5);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(m_transparent));
    painter->drawRect(option->rect);

    painter->restore();

    QGraphicsPixmapItem::paint(painter, option, widget);
}
