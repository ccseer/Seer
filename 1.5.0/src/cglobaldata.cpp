#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"
#include "types.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QRawFont>

CGlobalData::CGlobalData()
    : type(None),
      sub_type(S_Unknown),
      path(""),
      suffix(""),
      screen(NULL),
      is_resized_by_user(false),
      program_resizing(false)
{
    const QJsonDocument json_doc(QJsonDocument::fromJson(
        g_pini->GetIni(Ini::plugins_data_bytes).toByteArray()));
    if (json_doc.isNull()) {
        return;
    }
    const auto res = json_doc.array();
    if (res.isEmpty()) {
        return;
    }
    PluginData p_data;
    const auto res_t = res.count();
    for (int i = 0; i < res_t; ++i) {
        const auto arr = res.at(i).toArray();
        if (!arr.isEmpty() && arr.count() == 3) {
            p_data.name   = arr.at(PC_Name).toString();
            p_data.suffix = arr.at(PC_Suffix).toString().split(
                ' ', QString::SkipEmptyParts);
            p_data.para = arr.at(PC_Para).toString();

            if (p_data.name.isEmpty() || p_data.suffix.isEmpty()
                || p_data.para.isEmpty()) {
                continue;
            }
            plugins.append(p_data);
        }
    }
}

QFont fa::font(int sz)
{
    QFont ft(fa::FontMaterial);
    ft.setPixelSize(sz);
    ft.setStyleStrategy(ft.PreferAntialias);
    return ft;
}

QPixmap fa::img(fa::icon id, int sz, const QColor &color)
{
    const auto c = QChar(static_cast<int>(id));

    QFont ft = fa::font(sz);
    QFontMetrics fm(ft);
    QPixmap pix(fm.size(Qt::TextSingleLine, c));
    pix.fill("transparent");

    QPainter p(&pix);
    p.setRenderHints(p.Antialiasing | p.TextAntialiasing);
    p.setFont(ft);
    p.setPen(color);
    p.drawText(pix.rect(), Qt::AlignCenter, c);
    p.end();

    return pix;
}

QPixmap fa::img_choose(fa::icon id, int sz, const QColor &color)
{
    const auto c = QChar(static_cast<int>(id));

    QFont ft = fa::font(sz);
    QFontMetrics fm(ft);
    QPixmap pix(fm.size(Qt::TextSingleLine, c));
    pix.fill("transparent");

    auto rt           = QRawFont::fromFont(ft);
    auto indexes      = rt.glyphIndexesForString(fa::c(fa::ini_choose));
    QPainterPath path = rt.pathForGlyph(indexes.first());
    path.translate(0, pix.height());
    auto sp = path.toSubpathPolygons();

    QPainter p(&pix);
    p.setRenderHints(p.Antialiasing | p.TextAntialiasing);

    // circle
    p.setPen(color);
    p.setBrush(QBrush(color));
    p.drawPolygon(sp.last());

    p.setPen("white");
    p.setBrush(QBrush("white"));
    p.drawPolygon(sp.first());

    p.end();

    return pix;
}

QChar fa::c(fa::icon i)
{
    return QChar(static_cast<int>(i));
}
