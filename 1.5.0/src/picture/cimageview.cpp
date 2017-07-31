#include "cimageview.h"
#include "ccomponent.h"
#include "cgraphicspixmapitemwithbg.h"
#include "stable.h"
#include "utilities.h"

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include "QGraphicsProxyWidget.h"
#include "qaction.h"
#include "qimagereader.h"
#include "qmovie.h"
#include "qscrollbar.h"

CImageView::CImageView(QWidget *parent)
    : QGraphicsView(parent),
      m_pitem_preview(NULL),
      m_pitem_full(NULL),
      m_rotation(0),
      m_scale_factor(1)
{
    setFrameShape(NoFrame);
    setScene(&m_scene);
    m_scene.setItemIndexMethod(m_scene.NoIndex);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(SmartViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setResizeAnchor(AnchorViewCenter);
    setTransformationAnchor(AnchorViewCenter);

    InitActions();

    m_ani_rotate.setTargetObject(this);
    m_ani_rotate.setPropertyName("m_rotation");
    m_ani_rotate.setEasingCurve(QEasingCurve::InOutCubic);

    m_ani_scale.setDuration(200);
    m_ani_scale.setUpdateInterval(20);
    connect(&m_ani_scale, &QTimeLine::valueChanged, this,
            &CImageView::SlotScaling);
    connect(&m_ani_scale, &QTimeLine::finished, this,
            &CImageView::SlotScaleFinished);
}

CImageView::~CImageView()
{
    Cleanup();
}

void CImageView::InitActions()
{
    // ZoomIn, ZoomOut, ZoomRestore, Rotate90, Rotate270,
    QAction *act = new QAction(this);
    m_actions.insert(ActionType::ZoomIn, act);
    connect(act, &QAction::triggered, this, &CImageView::SlotZoomIn);

    act = new QAction(this);
    m_actions.insert(ActionType::ZoomOut, act);
    connect(act, &QAction::triggered, this, &CImageView::SlotZoomOut);

    act = new QAction(this);
    m_actions.insert(ActionType::ZoomRestore, act);
    connect(act, &QAction::triggered, this, &CImageView::SlotRestore);

    act = new QAction(this);
    m_actions.insert(ActionType::Rotate90, act);
    connect(act, &QAction::triggered, this, &CImageView::SlotRotate90);

    act = new QAction(this);
    m_actions.insert(ActionType::Rotate270, act);
    connect(act, &QAction::triggered, this, &CImageView::SlotRotate270);

    act = new QAction(this);
    act->setCheckable(true);
    act->setChecked(false);
    m_actions.insert(ActionType::SizeScale, act);
    connect(act, &QAction::triggered, this,
            [=](bool checked) { checked ? SlotSizeActual() : SlotSizeFit(); });
}

void CImageView::SetImage(const QImage &img)
{
    QPixmap pix     = QPixmap::fromImage(img);
    m_pitem_preview = new CGraphicsPixmapItemWithBg(pix);
    m_scene.addItem(m_pitem_preview);
    m_pitem_preview->setTransformOriginPoint(
        m_pitem_preview->boundingRect().center());
    m_scene.setSceneRect(QRect{{0, 0}, pix.size()});
    centerOn(m_pitem_preview);

    CheckRetoreAct();
}

void CImageView::SetImage(QMovie *mv)
{
    connect(mv, &QMovie::stateChanged, this, &CImageView::SigMovieStateChanged);

    auto label = new QLabel;
    label->setMovie(mv);
    label->setFixedSize(QImageReader(mv->fileName()).size());
    QGraphicsProxyWidget *item = m_scene.addWidget(label);
    m_scene.setSceneRect(QRect{{0, 0}, label->size()});
    centerOn(item);
    mv->start();

    CheckRetoreAct();
}

void CImageView::SlotZoomIn()
{
    if (m_ani_scale.state() == m_ani_scale.Running) {
        return;
    }
    m_is_zoom_in = false;
    m_ani_scale.start();
}

void CImageView::SlotZoomOut()
{
    if (m_ani_scale.state() == m_ani_scale.Running) {
        return;
    }
    //在上一次的基础上scale scale(0.5,0.5);
    m_is_zoom_in = true;
    m_ani_scale.start();
}

void CImageView::SlotRestore()
{
    if (m_pitem_preview) {
        // m_pitem_preview->setScale(1);
        m_pitem_preview->setTransform(QTransform().rotate(1), true);
        CheckRetoreAct();
    }
}

void CImageView::SlotRotate90()
{
    if (m_ani_rotate.state() == m_ani_rotate.Running) {
        return;
    }
    m_ani_rotate.setEndValue(GetRotation() + 90);
    m_ani_rotate.start();
}

void CImageView::SlotRotate270()
{
    if (m_ani_rotate.state() == m_ani_rotate.Running) {
        return;
    }
    m_ani_rotate.setEndValue(GetRotation() - 90);
    m_ani_rotate.start();
}

void CImageView::SlotFullImageReady(const QImage &pix)
{
    if (pix.isNull()) {
    }
    else {
        m_pitem_full = new CGraphicsPixmapItemWithBg(QPixmap::fromImage(pix));
        m_actions.value(SizeScale)->setEnabled(true);
    }
}

void CImageView::SlotSizeActual()
{
    if (m_pitem_preview) {
        m_scene.removeItem(m_pitem_preview);
    }
    if (m_pitem_full) {
        m_scene.addItem(m_pitem_full);
        m_scene.setSceneRect(QRect{{0, 0}, m_pitem_full->pixmap().size()});
    }
}

void CImageView::SlotSizeFit()
{
    if (m_pitem_full) {
        m_scene.removeItem(m_pitem_full);
    }
    if (m_pitem_preview) {
        m_scene.addItem(m_pitem_preview);
        m_scene.setSceneRect(QRect{{0, 0}, m_pitem_preview->pixmap().size()});
    }
}

void CImageView::Cleanup()
{
    if (m_pitem_preview) {
        cfree(m_pitem_preview);
    }

    if (m_pitem_full) {
        cfree(m_pitem_full);
    }
}

void CImageView::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void CImageView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void CImageView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void CImageView::CheckRetoreAct()
{
    // gif
    if (!m_pitem_preview) {
        foreach (auto i, m_actions.values()) {
            i->setEnabled(false);
        }
    }
    // image
    else {
        m_actions.value(ZoomRestore)->setEnabled(m_pitem_preview->scale() != 1);
        m_actions.value(ZoomIn)->setEnabled(m_pitem_preview->scale() <= 2.5);
        m_actions.value(ZoomOut)->setEnabled(m_pitem_preview->scale() >= 0.5);

        m_actions.value(SizeScale)->setEnabled(m_pitem_full);
    }

    GetAction(Rotate270)->setEnabled(true);
    GetAction(Rotate90)->setEnabled(true);
}

// rotation
void CImageView::ResetRotation()
{
    rotate(0);
    m_rotation = 0;
}

void CImageView::SetRotation(qreal angle)
{
    qreal diff = angle - m_rotation;
    m_rotation = angle;

    rotate(-diff);
}

QMovie *CImageView::GetMovie()
{
    const auto items = m_scene.items();
    if (!items.isEmpty()) {
        auto item = items.first();
        if (item->type() == QGraphicsProxyWidget::Type) {
            auto wnd = (QGraphicsProxyWidget *)item;
            if (auto label = (QLabel *)wnd->widget()) {
                if (auto mv = label->movie()) {
                    return mv;
                }
            }
        }
    }
    return NULL;
}

void CImageView::CopyContent2Clipboard()
{
    if (m_pitem_preview && m_scene.items().contains(m_pitem_preview)) {
        auto pix = m_pitem_preview->pixmap().transformed(this->transform());
        qApp->clipboard()->setPixmap(pix);
    }
    else if (m_pitem_full && m_scene.items().contains(m_pitem_full)) {
        auto pix = m_pitem_full->pixmap().transformed(this->transform());
        qApp->clipboard()->setPixmap(pix);
    }
    else {
        if (auto mv = GetMovie()) {
            auto pix = mv->currentPixmap().transformed(this->transform());
            qApp->clipboard()->setPixmap(pix);
        }
    }
}

void CImageView::SetMoviePlay(bool c)
{
    if (auto mv = GetMovie()) {
        mv->setPaused(!c);
    }
}

void CImageView::SeekMovieBackward()
{
    if (auto mv = GetMovie()) {
        if (mv->state() == mv->Paused) {
            const auto cur_f = mv->currentFrameNumber();
            int res_f        = cur_f - 1;
            if (cur_f == 0) {
                res_f = mv->frameCount() - 1;
            }
            mv->jumpToFrame(res_f);
        }
    }
}

void CImageView::SeekMovieForward()
{
    if (auto mv = GetMovie()) {
        if (mv->state() == mv->Paused) {
            const auto cur_f = mv->currentFrameNumber();
            int res_f        = cur_f + 1;
            if (cur_f == mv->frameCount() - 1) {
                res_f = 0;
            }
            mv->jumpToFrame(res_f);
        }
    }
}

// Scale
void CImageView::SlotScaleFinished()
{
    m_is_zoom_in ? m_scale_factor += 0.3 : m_scale_factor -= 0.3;
    // 限制某些情况下的缩放 , 缩放暂时隐藏
}

void CImageView::SlotScaling(qreal x)
{
    qreal factor_once
        = qreal((m_is_zoom_in ? 1 : -1) * x * 0.3 + m_scale_factor);
    QTransform t;
    t.scale(factor_once, factor_once);
    t.rotate(-GetRotation());
    setTransform(t);
}

void CImageView::wheelEvent(QWheelEvent *e)
{
    if (auto scr = this->verticalScrollBar()) {
        if (e->modifiers().testFlag(Qt::ControlModifier)) {
            if (GetAction(SizeScale)->isEnabled()) {
                const int numDegrees = e->delta() / 8;
                const int numSteps   = numDegrees / 15;
                int step             = 3 * numSteps * scr->singleStep();
                if (step > 0) {
                    if (!GetAction(SizeScale)->isChecked()) {
                        const QPoint be4 = e->pos();
                        const qreal ratio_x
                            = qreal(be4.x())
                              / m_pitem_preview->pixmap().width();
                        const qreal ratio_y
                            = qreal(be4.y())
                              / m_pitem_preview->pixmap().height();
                        GetAction(SizeScale)->trigger();
                        this->centerOn(
                            ratio_x * m_pitem_full->pixmap().width(),
                            ratio_y * m_pitem_full->pixmap().height());
                    }
                }
                else if (step < 0) {
                    if (GetAction(SizeScale)->isChecked()) {
                        GetAction(SizeScale)->trigger();
                    }
                }
            }
        }
        else {
            if (scr->isVisible()) {
                const int numDegrees = e->delta() / 8;
                const int numSteps   = numDegrees / 15;
                scr->setValue(scr->value() - 3 * numSteps * scr->singleStep());
            }
        }
    }
}
