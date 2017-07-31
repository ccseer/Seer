#include "cscreenadapter.h"
#include <QFile>
#include <QScreen>
#include <QTimer>

#include "stable.h"

const QChar g_qss_marker = '@';

// logicalDpi: Resolution used for converting font sizes defined in points to
// font sizes in pixels. Typically one of the standard values 96, 128, .. 192.
// physicalDpiX: Physical resolution obtained by dividing the size of the
// monitor by the number of pixels.

// logicalDpiX       physicalDpiX
// 96                102
// 144               102         // 1.5倍缩放
// 96                57          // 800*600
// 120               72          // hdpi
// 72                114         // mac

// const qreal g_dpi_ref = 96.;
// const qreal g_width_ref = 1920.;
// const qreal g_height_ref = 1080.;

CScreenAdapter::CScreenAdapter(QWindow *wnd)
    : QObject(wnd), m_ratio(1), m_associated_wnd(NULL)
{
    DoConnect(wnd);
}

CScreenAdapter::CScreenAdapter() : m_ratio(1), m_associated_wnd(NULL)
{
}

void CScreenAdapter::init(QWidget *wnd)
{
    if (m_associated_wnd) {
        return;
    }

    auto w = getTopWnd(wnd);
    DoConnect(w->windowHandle());
}

void CScreenAdapter::init(QWindow *wnd)
{
    DoConnect(wnd);
}

qreal CScreenAdapter::x()
{
    return getSize(3);
}

qreal CScreenAdapter::getSize(qreal sz)
{
    return sz * m_ratio;
}

QWidget *CScreenAdapter::getTopWnd(QWidget *w)
{
    QWidget *p = w;
    while (p) {
        auto backup = p;
        p           = p->parentWidget();
        if (!p) {
            p = backup;
            break;
        }
    }
    return p;
}

void CScreenAdapter::DoConnect(QWindow *wnd)
{
    if (m_associated_wnd) {
        return;
    }
    m_associated_wnd = wnd;

    connect(wnd, &QWindow::screenChanged, this, &CScreenAdapter::ScreenChanged,
            Qt::UniqueConnection);
    connect(wnd->screen(), &QScreen::logicalDotsPerInchChanged, this,
            [=]() {
                if (auto s = qobject_cast<QScreen *>(sender())) {
                    ScreenChanged(s);
                }
            },
            Qt::UniqueConnection);

    connect(wnd->screen(), &QScreen::geometryChanged, this,
            [=]() {
                if (auto s = qobject_cast<QScreen *>(sender())) {
                    ScreenChanged(s);
                }
            },
            Qt::UniqueConnection);
    connect(wnd, &QWindow::visibilityChanged, this,
            [=](QWindow::Visibility v) {
                if (v == QWindow::Windowed || v == QWindow::Maximized
                    || v == QWindow::FullScreen) {
                    ScreenChanged(wnd->screen());
                }
            },
            Qt::UniqueConnection);

    ScreenChanged(wnd->screen());
}

qreal CScreenAdapter::getRatio(QWidget *w)
{
    auto s = w->windowHandle()->screen();
    return getRatio(s);
}

qreal CScreenAdapter::getRatio(QScreen *s)
{
    qreal ratio = 1.;

#ifdef Q_OS_MAC
    Q_UNUSED(s)
    return ratio;
#else
    const auto dpi = s->logicalDotsPerInch();
    if (dpi <= 108) {  // 0-96-108
        ratio = 1;
    }
    else if (dpi <= 132) {  // 108-120-132
        ratio = 1.25;
    }
    else if (dpi <= 168) {  // 132-144-168
        ratio = 1.5;
    }
    else {  // 168-192-inf
        ratio = 2;
    }
    return ratio;
#endif
}

QString UpdateQSSHelper(const QString &line, qreal r)
{
    auto bak     = line;
    auto index   = line.indexOf(g_qss_marker);
    auto px_mark = line.mid(index, line.lastIndexOf(g_qss_marker) - index + 1);
    auto px = line.mid(index + 1, line.lastIndexOf(g_qss_marker) - index - 1);
    const bool has_px = px.contains("px");
    if (has_px) {
        px.remove("px");
    }
    auto new_sz = QString::number(px.toFloat() * r);
    if (has_px) {
        new_sz += "px";
    }
    bak.replace(px_mark, new_sz);
    return bak;
}

QString CScreenAdapter::UpdateQSS(const QString &qss_path, qreal ratio)
{
    QFile f(qss_path);
    if (!f.open(f.ReadOnly)) {
        elprint << f.errorString();
        return "";
    }
    QString qss;
    QString line;

    while (!f.atEnd()) {
        line           = f.readLine();
        const auto m_t = line.count(g_qss_marker);
        switch (m_t) {
            case 0:
                qss.append(line);
                break;
            case 2: {
                // @-6px@
                //  @5@
                //  @4.5@
                qss.append(UpdateQSSHelper(line, ratio));
                break;
            }
            case 4:
            case 8:
            case 6: {
                // padding: @6px@ @27px@ @6px@ @36px@;
                QString res;
                foreach (const QString &i, line.split(" ")) {
                    if (i.contains(g_qss_marker)) {
                        res.append(UpdateQSSHelper(i, ratio));
                    }
                    else {
                        res.append(i);
                    }
                    res.append(" ");
                }
                qss.append(res);
                break;
            }
            default:
                qprintl << m_t << line;
                break;
        }
    }
    f.close();
    if (qss.contains(g_qss_marker)) {
        elprintf;
    }

    return qss;
}

QString CScreenAdapter::UpdateQSS(const QString &qss_path)
{
    return UpdateQSS(qss_path, m_ratio);
}

void CScreenAdapter::ScreenChanged(QScreen *screen)
{
    const qreal ratio = getRatio(screen);

    if (m_ratio != ratio) {
        m_ratio = ratio;
        QTimer::singleShot(0, this, [=]() { emit ratioChanged(m_ratio); });
    }

    /// @note http://doc.qt.io/qt-5/scalability.html
    //    const QRect rt = screen->geometry();
    //    const auto width = qMax(rt.width(), rt.height());
    //    const auto height = qMin(rt.width(), rt.height());

    //    const qreal dpi = screen->logicalDotsPerInch();
    //    const qreal ratio_new = qMin(height/g_height_ref, width/g_width_ref);
    //    if (!qFuzzyCompare(ratio_new, m_ratio)) {
    //        qprint<<"new ratio"<<m_ratio<< ratio_new;
    //        m_ratio = ratio_new;
    //        emit ratioChanged(m_ratio);
    //    }

    //    const qreal new_ratio_f = qMin(height*g_dpi_ref/(dpi*g_height_ref),
    //    width*g_dpi_ref/(dpi*g_width_ref));
    //    if (!qFuzzyCompare(new_ratio_f, m_ratio_f)) {
    //        qprint<<"new ratio font"<<m_ratio_f<< new_ratio_f;
    //        m_ratio_f = new_ratio_f;
    //        emit ratioFontChanged(m_ratio_f);
    //    }
}
