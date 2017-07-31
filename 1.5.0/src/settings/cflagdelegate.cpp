#include "cflagdelegate.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "stable.h"

#include <QPainter>
#include <QStandardItemModel>

#define g_star_len 15 * m_ratio
#define g_flag_len 39 * m_ratio
#define g_space 18 * m_ratio

CFlagDelegate::CFlagDelegate(QObject *parent)
    : QStyledItemDelegate(parent), m_ratio(1)
{
    UpdateDpi(m_ratio);
}

void CFlagDelegate::UpdateDpi(qreal r)
{
    m_ratio    = r;
    m_pix_star = fa::img_choose(fa::ini_choose, g_star_len, "#0288d1");
}

qreal CFlagDelegate::GetFlagBaseSz()
{
    return g_flag_len;
}

void CFlagDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    auto icon = index.data(g_orig_icon_role).value<QIcon>();
    // 没有 flag
    if (icon.isNull()) {
        painter->drawText(option.rect, Qt::TextWrapAnywhere | Qt::AlignCenter,
                          index.data().toString());
    }
    else {
        painter->drawPixmap(
            QRect(option.rect.x(), option.rect.y(), g_flag_len, g_flag_len),
            icon.pixmap(g_flag_len, g_flag_len));
    }

    if (index.data(g_cur_role) == true) {
        painter->drawPixmap(QRect(option.rect.x() + g_flag_len - g_star_len / 2,
                                  option.rect.y(), g_star_len, g_star_len),
                            m_pix_star);
    }
    painter->restore();
}

QSize CFlagDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(g_flag_len + g_star_len / 2 + g_space,
                 g_flag_len + g_star_len / 2);
}
