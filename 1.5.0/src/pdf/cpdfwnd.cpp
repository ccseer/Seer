#include "cpdfwnd.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "utilities.h"

#ifdef PDF
#include <QActionGroup>
#include <QDesktopWidget>
#include <QToolButton>
#include "qaction.h"
#include "qlabel.h"
#include "qlayout.h"

// TODO: Enchance: 宽度有问题
#define page_margin fontMetrics().height() * 20

CPdfWnd::CPdfWnd(QWidget *parent /*= 0*/)
    : CAbstractWnd(ManagerPtr(new CAbstractManager), parent)
{
    SetWnd(&m_view);
    m_view.setFrameShape(m_view.NoFrame);
    m_view.setMouseTool(PdfView::TextSelection);
    m_view.setMaximumFileSettingsCacheSize(1024 * 1024 * 250);
    m_view.scene()->setBackgroundBrush(g_pstyle->GetSceneBackgroundColor());
}

CPdfWnd::~CPdfWnd()
{
    UpdateRecents();
    m_view.close();
}

void CPdfWnd::InitControlWnd(QHBoxLayout *lay)
{
    // page num
    {
        m_plabel_page = new QLabel;
        m_plabel_page->setToolTip(tr("Pages Count"));
        m_plabel_page->setObjectName("ctrlbar_big_label");
        connect(
            &m_view, &PdfView::scrollPositionChanged,
            [=](qreal /*fraction*/, int pageNumber) {
                m_plabel_page->setText(
                    QString("%1/%2")
                        .arg(QString::number(pageNumber + 1))
                        .arg(QString::number(m_view.document()->numPages())));
            });
    }

    this->addAction(m_view.action(PdfView::GoToPreviousPage));
    this->addAction(m_view.action(PdfView::GoToNextPage));

    auto act = m_view.action(PdfView::GoToStartOfDocument);
    act->setShortcut(QKeySequence("Home"));
    this->addAction(act);
    act = m_view.action(PdfView::GoToEndOfDocument);
    act->setShortcut(QKeySequence("End"));
    this->addAction(act);

    auto btn_page
        = GetBottomBtn(fa::ctrl_pagewidth, tr("Fit Width"),
                       g_pini->GetIni(Ini::key_pdf_fit_width_str).toString());
    btn_page->defaultAction()->setCheckable(true);
    this->addAction(btn_page->defaultAction());

    m_pbtn_width
        = GetBottomBtn(fa::ctrl_width_blue, tr("Actual Size"),
                       g_pini->GetIni(Ini::key_pdf_actual_sz_str).toString());
    m_pbtn_width->defaultAction()->setCheckable(true);
    this->addAction(m_pbtn_width->defaultAction());

    QActionGroup *grp = new QActionGroup(this);
    grp->setExclusive(true);
    grp->addAction(btn_page->defaultAction());
    grp->addAction(m_pbtn_width->defaultAction());
    if (g_pini->GetIni(Ini::pdf_is_actualsize_view_b).toBool()) {
        m_pbtn_width->defaultAction()->setChecked(1);
    }
    else {
        btn_page->defaultAction()->setChecked(1);
    }

    connect(btn_page->defaultAction(), &QAction::triggered, [=](bool checked) {
        if (checked) {
            // 窗口大小 / 当前view大小
            const auto zoom = size().width() * 1.00
                              / int(m_view.sceneRect().size().toSize().width()
                                    / m_view.zoomFactor());
            if (!qFuzzyCompare(m_view.zoomFactor(), zoom)) {
                m_view.setZoomFactor(zoom);
                g_pini->SetIni(Ini::pdf_is_actualsize_view_b, false);
            }
        }
    });
    connect(m_pbtn_width->defaultAction(), &QAction::triggered,
            [=](bool checked) {
                if (checked) {
                    if (m_view.zoomFactor() != 1) {
                        m_view.setZoomFactor(1);
                    }
                    g_pini->SetIni(Ini::pdf_is_actualsize_view_b, true);
                }
            });

    lay->addWidget(btn_page);
    lay->addWidget(m_pbtn_width);
    lay->addStretch();
    lay->addWidget(m_plabel_page);
}

FilePropertyData CPdfWnd::LoadFileInfoHelper()
{
    FilePropertyData res;
    res.append(FileProperty{tr("Pages Count"),
                            QString::number(m_view.document()->numPages())});
    foreach (const auto &i, m_view.document()->infoKeys()) {
        res.append(FileProperty{i, m_view.document()->info(i)});
    }
    return res;
}

void CPdfWnd::UpdateRecents()
{
    if (g_data->path.isEmpty()) {
        return;
    }

    // QList<QVariantList>  <path,pos>
    QVariantList recents_list = g_pini->GetIni(Ini::pdf_recents_list).toList();
    foreach (const QVariant &i, recents_list) {
        const auto interal = i.toList();
        if (interal[0] == g_data->path) {
            recents_list.removeOne(interal);
            break;
        }
    }
    recents_list.prepend(QVariant::fromValue(
        QVariantList{g_data->path, m_view.pageNumberWithPosition()}));
    if (recents_list.size() > g_pdf_recents_max) {
        recents_list = recents_list.mid(0, g_pdf_recents_max);
    }

    g_pini->SetIni(Ini::pdf_recents_list, recents_list);
}

void CPdfWnd::RestoreFromRecents()
{
    foreach (const QVariant &i,
             g_pini->GetIni(Ini::pdf_recents_list).toList()) {
        const auto interal = i.toList();
        if (interal[0] == g_data->path) {
            const auto last = interal[1].toDouble();
            if (last != m_view.pageNumberWithPosition()) {
                m_view.setPage(last);
            }
            break;
        }
    }
}

QSize CPdfWnd::GetSize()
{
    int height      = 90000;
    const auto list = m_view.popplerPages();
    if (list.size() == 1) {
        height = list.first()->pageSize().height() + page_margin;
    }
    else if (list.size() == 0) {
        height = GetMinWndSz(m_dpi->ratio()).height();
    }

    auto wid = int(m_view.sceneRect().width());
    if (!g_pini->GetIni(Ini::pdf_is_actualsize_view_b).toBool()) {
        return QSize{wid, height};
    }
    return QSize{wid + page_margin, height};
}

void CPdfWnd::LoadHelper()
{
    // 窗口大小 / 当前view大小
    if (!g_pini->GetIni(Ini::pdf_is_actualsize_view_b).toBool()) {
        QScopedPointer<Poppler::Document> popplerDocument(
            Poppler::Document::load(g_data->path));
        if (popplerDocument.isNull()) {
            return;
        }
        QScopedPointer<Poppler::Page> page(popplerDocument->page(0));
        if (page.isNull()) {
            return;
        }
        const auto page_w = page->pageSizeF().width();

        const auto width_view = (page_w + g_interPageSpace)
                                    * QApplication::desktop()->physicalDpiX()
                                    * 1.00 / 72.0
                                + 2;
        const auto width_wnd = width_view + page_margin;
        const auto max_width = GetMaxContentSize(g_data->screen).width();
        if (width_wnd > max_width) {
            m_view.setZoomFactor(max_width * 1.00 / width_view);
        }
        else {
            m_view.setZoomFactor(width_wnd * 1.00 / width_view);
        }
    }

    if (m_view.load(g_data->path)) {
        emit SigLoaded();
        RestoreFromRecents();
    }
    else {
        emit SigErr();
    }
}

#endif  // PDF
