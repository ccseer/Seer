#include "caboutwnd.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cparentdlg.h"
#include "stable.h"
#include "ui_caboutwnd.h"
#include "utilities.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include "qpropertyanimation.h"
#include "qurl.h"

CAboutWnd::CAboutWnd(QWidget *parent)
    : QWidget(parent),
      m_wnd_pressed(false),
      m_movable(true),
      ui(new Ui::CAboutWnd)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_info);

    // value
    ui->label_head_text_sub->setToolTip(web_relnote_all);
    ui->label_me_support->setToolTip(qApp->organizationDomain());
    ui->label_head_text_sub->setText(tr("Version")
                                     + QString(" %1").arg(VERSION));
    ui->label_me_name->setText("@Corey");
    ui->label_me_name->setToolTip(web_corey_twitter);
    ui->label_designer->setToolTip(web_designer_website);
    ui->label_translator->setToolTip(web_translator);
    if (QLocale::system().country() == QLocale::China) {
        ui->label_me_name->setToolTip(web_corey_weibo);
    }

    connect(ui->label_head, &CClickCheckingLabel::SigClicked, this,
            &CAboutWnd::SlotBonus);

    // style
    foreach (auto i, this->findChildren<QFrame *>()) {
        if (i->objectName().startsWith("line")) {
            i->setObjectName("line");
        }
    }
    foreach (auto i, ui->page_info->findChildren<QLabel *>()) {
        if (i == ui->label_title_text || i == ui->label_head_text
            || i == ui->label_head || i == ui->label_title_links
            || i == ui->label_title_me) {
            continue;
        }
        if (!i->objectName().contains("label_dot")) {
            i->setCursor(Qt::PointingHandCursor);
            i->installEventFilter(this);
            m_clickable_label.append(i);
            if (ui->label_head_text_sub != i) {
                i->setObjectName("label_clickable");
            }
        }
        else {
            i->setObjectName("label_dot");
        }
    }
    ui->label_title_links->setObjectName("label_grp_title");
    ui->label_title_me->setObjectName("label_grp_title");
}

void CAboutWnd::UpdateDPI(qreal r)
{
    ui->pushButton_close->setFont(fa::font(18 * r));
    ui->pushButton_close->setText(fa::c(fa::close));

    QPixmap pix("://icons/256.png");
    pix = pix.scaledToWidth(50 * 3 * r, Qt::SmoothTransformation);
    ui->label_head->setPixmap(pix);

    // width
    {
        int max_link_width = 0;
        const QList<QLabel *> links{ui->label_designer, ui->label_translator};
        foreach (auto i, links) {
            max_link_width
                = qMax(i->fontMetrics().width(i->text()), max_link_width);
        }
        max_link_width
            += ui->label_designer->fontMetrics().averageCharWidth() * 2;
        foreach (auto i, links) {
            i->setFixedWidth(max_link_width);
        }

        const auto fm  = ui->label_title_links->fontMetrics();
        max_link_width = qMax(fm.width(ui->label_title_links->text()),
                              fm.width(ui->label_title_me->text()))
                         + fm.averageCharWidth() * 2;
        ui->label_title_me->setFixedWidth(max_link_width);
        ui->label_title_links->setFixedWidth(max_link_width);
    }

    // margin & space
    {
        const auto hei    = 17 * r;
        const auto space1 = hei * 2;
        const auto space2 = hei * 1.2;
        const auto space3 = hei * 0.7;

        ui->Layout_title->setContentsMargins(space2, space2, space2, 0);

        ui->Layout_all->setSpacing(space1);

        ui->Layout_logo_container->setSpacing(space3);
        ui->Layout_links_container->setSpacing(space2);
        ui->Layout_me_container->setSpacing(space2);

        ui->wnd_links_content->setSpacing(space3);

        ui->Layout_me_title->setContentsMargins(space2, 0, space2, 0);
        ui->Layout_links_title->setContentsMargins(
            ui->Layout_me_title->contentsMargins());
    }
}

void CAboutWnd::OpenUrlWithChecking(const QUrl &url)
{
    if (!OpenByDefaultApp(url)) {
        emit SigOpenUrlFailed();
    }
}

CAboutWnd::~CAboutWnd()
{
    delete ui;
}

void CAboutWnd::mousePressEvent(QMouseEvent *e)
{
    // move wnd
    if (m_movable && e->button() == Qt::LeftButton
        && ui->wnd_title_x->geometry().contains(
               ui->wnd_title_x->mapFromGlobal(e->globalPos()))) {
        if (auto wnd = GetParentDlgPtr) {
            m_wnd_move    = e->globalPos() - wnd->frameGeometry().topLeft();
            m_wnd_pressed = true;
        }
    }

    QWidget::mousePressEvent(e);
}

void CAboutWnd::mouseReleaseEvent(QMouseEvent *e)
{
    m_wnd_pressed = false;
    QWidget::mouseReleaseEvent(e);
}

void CAboutWnd::mouseMoveEvent(QMouseEvent *e)
{
    if (m_wnd_pressed) {
        if (auto wnd = GetParentDlgPtr) {
            wnd->move(e->globalPos() - m_wnd_move);
        }
    }
    QWidget::mouseMoveEvent(e);
}

bool CAboutWnd::eventFilter(QObject *o, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonRelease) {
        if (auto label = qobject_cast<QLabel *>(o)) {
            if (m_clickable_label.contains(label)) {
                if (label == ui->label_me_email) {
                    OpenUrlWithChecking(
                        QUrl("mailto:" + ui->label_me_email->text()));
                }
                else {
                    OpenUrlWithChecking(QUrl(label->toolTip()));
                }
            }
        }
    }
    return false;
}

void CAboutWnd::on_pushButton_close_clicked()
{
    if (auto dlg = GetParentDlgPtr) {
        dlg->on_pushButton_close_clicked();
    }
}

void CAboutWnd::SlotBonus()
{
    if (!m_movable) {
        return;
    }

    CParentDlg *dlg = GetParentDlgPtr;
    if (!dlg) {
        return;
    }

    m_movable = false;
    ui->label_me_support->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->label_head->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto wnd              = ui->label_head;
    const auto pix_backup = wnd->pixmap()->copy();
    QPixmap pix_white{pix_backup.size()};
    pix_white.fill(Qt::transparent);

    QLabel *bonus = new QLabel;
    connect(this, &QObject::destroyed, bonus, &QLabel::deleteLater);
    bonus->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    bonus->setAttribute(Qt::WA_TranslucentBackground);
    bonus->setAttribute(Qt::WA_TransparentForMouseEvents);
    bonus->setPixmap(pix_backup);
    bonus->move(wnd->mapToGlobal(QPoint(0, 0)));
    bonus->show();
    bonus->clearFocus();
    wnd->setPixmap(pix_white);

    const QEasingCurve::Type fast = QEasingCurve::InQuart;
    const QEasingCurve::Type slow = QEasingCurve::OutQuart;
    const auto height_half        = qApp->fontMetrics().xHeight() * 20;
    const ushort DURATION         = 700;

    QSequentialAnimationGroup *grp = new QSequentialAnimationGroup;
    // #1
    QPropertyAnimation *ani_pos = new QPropertyAnimation(bonus, "pos", grp);
    const QPoint distance2dlg   = wnd->mapTo(dlg, QPoint{0, 0});
    ani_pos->setEndValue(
        bonus->pos() - QPoint(distance2dlg.x() + bonus->width(), -height_half));
    ani_pos->setDuration(DURATION);
    ani_pos->setEasingCurve(fast);
    connect(ani_pos, &QPropertyAnimation::finished, this, [=]() {
        dlg->activateWindow();
        dlg->raise();
        bonus->clearFocus();
    });

    // #2
    QPropertyAnimation *ani_pos21 = new QPropertyAnimation(bonus, "pos", grp);
    ani_pos21->setEndValue(wnd->mapToGlobal(QPoint(0, 0))
                           + QPoint{0, height_half * 4});
    ani_pos21->setDuration(DURATION);
    ani_pos21->setEasingCurve(slow);

    QPropertyAnimation *ani_pos22 = new QPropertyAnimation(bonus, "pos", grp);
    ani_pos22->setEndValue(
        QPoint{dlg->size().width() + dlg->pos().x(),
               wnd->mapToGlobal(QPoint(0, 0)).y() + height_half});
    ani_pos22->setDuration(DURATION);
    ani_pos22->setEasingCurve(fast);
    connect(ani_pos22, &QPropertyAnimation::finished, this, [=]() {
        bonus->setWindowFlags(bonus->windowFlags() | Qt::WindowStaysOnTopHint);
        bonus->show();
    });

    // #3
    QPropertyAnimation *ani_pos3 = new QPropertyAnimation(bonus, "pos", grp);
    ani_pos3->setEndValue(wnd->mapToGlobal(QPoint(0, 0)));
    ani_pos3->setDuration(DURATION);
    ani_pos3->setEasingCurve(slow);
    connect(ani_pos3, &QPropertyAnimation::finished, this, [=]() {
        wnd->setPixmap(pix_backup);
        ui->label_me_support->setAttribute(Qt::WA_TransparentForMouseEvents,
                                           false);
        ui->label_head->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_movable = true;
        bonus->deleteLater();
    });

    //    qprint<<ani_pos->easingCurve().type()<<ani_pos3->easingCurve().type();
    //    qprint<<ani_pos->endValue()<<ani_pos21->endValue()<<ani_pos22->endValue()<<ani_pos3->endValue();
    grp->addAnimation(ani_pos);
    grp->addAnimation(ani_pos21);
    grp->addAnimation(ani_pos22);
    grp->addAnimation(ani_pos3);
    grp->start(grp->DeleteWhenStopped);
}
