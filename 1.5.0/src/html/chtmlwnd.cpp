#include "chtmlwnd.h"
#include "ccodewnd.h"
#include "chtmlmanager.h"
#include "cthememanager.h"
#include "extensions.h"
#include "settingshelper.h"
#include "utilities.h"

#include <qevent.h>
#include <QAction>
#include <QActionGroup>
#include <QFile>
#include <QWebEngineProfile>

CHtmlWnd::CHtmlWnd(QWidget *parent)
    : CAbstractWnd(ManagerPtr(new CHtmlManager), parent),
      m_pwnd_web(new QWebEngineView),
      m_pwnd_container(new QStackedWidget),
#ifdef HIGHLIGHT
      m_pwnd_html(new QsciScintilla),
#endif
      m_is_first(true)
{
    InitWeb();
    InitHtmlCodeWnd();
    m_pwnd_container->addWidget(m_pwnd_web);
#ifdef HIGHLIGHT
    m_pwnd_container->addWidget(m_pwnd_html);
#endif
    SetWnd(m_pwnd_container);
    m_pwnd_container->setCurrentWidget(m_pwnd_web);
}

void CHtmlWnd::InitControlWnd(QHBoxLayout *lay)
{
#ifdef HIGHLIGHT
    m_pbtn_html = GetBottomBtn(fa::ctrl_html, tr("HTML"));
    m_pbtn_html->defaultAction()->setCheckable(true);
    m_pbtn_html->defaultAction()->setEnabled(false);
    lay->addWidget(m_pbtn_html);
    lay->addStretch();

    connect(m_pbtn_html->defaultAction(), &QAction::triggered, this,
            [=](bool checked) {
                if (checked) {
                    m_pwnd_container->setCurrentWidget(m_pwnd_html);
                    LoadHtmlCode();
                }
                else {
                    m_pwnd_container->setCurrentWidget(m_pwnd_web);
                }
            });
#endif
}

CHtmlWnd::~CHtmlWnd()
{
    m_ppage->disconnect();
    //  程序会走到析构时,又跳回到 SlotWebLoaded(false) 处, 然后崩溃
    m_pwnd_web->disconnect();
}

void CHtmlWnd::InitWeb()
{
    using SET     = QWebEngineSettings;
    auto settings = SET::globalSettings();
    settings->setFontFamily(SET::StandardFont, qApp->font().family());
    settings->setAttribute(SET::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(SET::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(SET::LocalStorageEnabled, true);
    settings->setAttribute(SET::JavascriptEnabled, true);
    settings->setAttribute(SET::JavascriptCanAccessClipboard, false);
    settings->setAttribute(SET::AutoLoadImages, true);

    // settings->setAttribute(SET::DeveloperExtrasEnabled, true);
    m_pwnd_web->setContextMenuPolicy(Qt::NoContextMenu);
    m_ppage = new CWebPage(this);
    m_pwnd_web->setPage(m_ppage);
    m_ppage->setZoomFactor(g_pini->GetIni(Ini::web_zoom_size_d).toReal());
    connect(m_ppage, &CWebPage::loadFinished, this, &CHtmlWnd::SlotWebLoaded);
    connect(m_ppage, &CWebPage::SigLinkClicked, m_ppage, [=](const QUrl &url) {
        if (!OpenByDefaultApp(url)) {
            emit SigWiggleMainWnd();
        }
    });
    m_ppage->view()->installEventFilter(this);
}

void CHtmlWnd::LoadHtmlCode()
{
#ifdef HIGHLIGHT
    if (m_pwnd_html->text().isEmpty()) {
        m_pwnd_web->page()->toHtml([=](const QString &code) {
            qApp->setOverrideCursor(Qt::WaitCursor);
            m_pwnd_html->setText(code);
            qApp->restoreOverrideCursor();
        });
    }
#endif
}

void CHtmlWnd::InitHtmlCodeWnd()
{
#ifdef HIGHLIGHT
    m_pwnd_html->viewport()->installEventFilter(this);

    CCodeWnd::InitFormatWnd(m_pwnd_html);
    CCodeWnd::UpdateFormatLaxer(m_pwnd_html, S_Html);
#endif
}

void CHtmlWnd::PerformFinished(const QVariant &v)
{
    //传出的 web 的 code text
    auto res = v.toString();
    if (!GetErrMsg().isEmpty() || res.isEmpty()) {
        emit SigErr();
        return;
    }

    m_pwnd_web->load(QUrl::fromLocalFile(res));
    m_pwnd_web->update();
    m_pwnd_web->page()->view()->update();
    emit SigLoaded();
}

void CHtmlWnd::SlotWebLoaded(bool ok)
{
    if (m_is_first) {
        m_is_first = false;
        if (ok) {
            m_pbtn_html->defaultAction()->setEnabled(true);
            m_pwnd_web->update();
            m_pwnd_web->page()->view()->update();
        }
        else {
            emit SigErr();
        }
    }
}

QSize CHtmlWnd::GetSize()
{
    return QSizeF{13 * 70 * m_dpi->ratio(), 13 * 55 * m_dpi->ratio()}.toSize();
}

bool CHtmlWnd::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() != QEvent::Wheel) {
        return false;
    }
    QWheelEvent *e = (QWheelEvent *)ev;
    if (e->modifiers() != Qt::ControlModifier
        || (m_pwnd_html->viewport() != obj && m_ppage->view() != obj)) {
        return false;
    }

    const bool is_web = (m_ppage->view() == obj);
    if (e->delta() > 0) {
        if (!is_web) {
            if (m_pwnd_html->SendScintilla(m_pwnd_html->SCI_GETZOOM) >= 20) {
                return true;
            }
            m_pwnd_html->zoomIn();
        }
        else {
            if (m_ppage->zoomFactor() >= 5) {
                return true;
            }
            m_ppage->setZoomFactor(m_ppage->zoomFactor() + 0.2);
        }
    }
    else {
        if (!is_web) {
            if (m_pwnd_html->SendScintilla(m_pwnd_html->SCI_GETZOOM) <= -2) {
                return true;
            }
            m_pwnd_html->zoomOut();
        }
        else {
            if (m_ppage->zoomFactor() <= -2) {
                return true;
            }
            m_ppage->setZoomFactor(m_ppage->zoomFactor() - 0.2);
        }
    }

    if (!is_web) {
        g_pini->SetIni(Ini::text_zoom_size_i,
                       m_pwnd_html->SendScintilla(m_pwnd_html->SCI_GETZOOM));
    }
    else {
        g_pini->SetIni(Ini::web_zoom_size_d, m_ppage->zoomFactor());
    }
    return true;
}
