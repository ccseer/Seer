#ifndef CIMAGEVIEW_H
#define CIMAGEVIEW_H

#include <QGraphicsView>
#include <QMovie>
#include <QTimeLine>
#include "qpropertyanimation.h"

class CGraphicsPixmapItemWithBg;

class CImageView : public QGraphicsView {
    Q_OBJECT
    Q_PROPERTY(
        qreal m_rotation READ GetRotation WRITE SetRotation RESET ResetRotation)

public:
    enum ActionType {
        // smaller
        ZoomOut,
        // bigger
        ZoomIn,
        ZoomRestore,

        Rotate90,
        Rotate270,

        SizeScale,
    };

    CImageView(QWidget *parent = 0);
    ~CImageView();

    QAction *GetAction(ActionType t) const
    {
        return m_actions.value(t);
    }

    void SetRotation(qreal angle);
    qreal GetRotation()
    {
        return m_rotation;
    }

protected:
    void enterEvent(QEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *);

private:
    void Cleanup();
    void InitActions();
    QMovie *GetMovie();

    CGraphicsPixmapItemWithBg *m_pitem_preview;
    CGraphicsPixmapItemWithBg *m_pitem_full;
    QGraphicsScene m_scene;
    QHash<ActionType, QAction *> m_actions;

    qreal m_rotation;
    QPropertyAnimation m_ani_rotate;

    qreal m_scale_factor;
    bool m_is_zoom_in;
    QTimeLine m_ani_scale;

public:
signals:
    void SigMovieStateChanged(QMovie::MovieState state);

public slots:
    void SlotFullImageReady(const QImage &pix);

    void SetImage(const QImage &pix);
    void SetImage(QMovie *mv);

    void CheckRetoreAct();

    void SlotZoomIn();
    void SlotZoomOut();

    void SlotRestore();
    void SlotRotate90();
    void SlotRotate270();

    void SlotSizeActual();
    void SlotSizeFit();

    void ResetRotation();

    void SetMoviePlay(bool c);
    void SeekMovieBackward();
    void SeekMovieForward();

    void CopyContent2Clipboard();

private slots:
    void SlotScaleFinished();
    void SlotScaling(qreal x);
};

#endif  // CIMAGEVIEW_H
