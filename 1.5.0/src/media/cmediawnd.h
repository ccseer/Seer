#ifndef CMEDIAWND_H
#define CMEDIAWND_H

#include "cabstractwnd.h"
#include "cglobaldata.h"

#ifdef MEDIA
#include <QtAV/QtAV.h>
#include <QtAV/SubtitleFilter.h>
#include <QtAVWidgets/OpenGLWidgetRenderer.h>
#include <QtAVWidgets/WidgetRenderer.h>

class QStackedWidget;
class QPushButton;
class QSlider;
class QLabel;

class CMediaWnd : public CAbstractWnd {
    Q_OBJECT
public:
    explicit CMediaWnd(QWidget *parent = 0);
    ~CMediaWnd();

    virtual QSize GetSize();

    virtual DisplayType Type() const
    {
        return Media;
    }

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

    virtual void LoadHelper();

    virtual void InitControlWnd(QHBoxLayout *lay);

    virtual FilePropertyData LoadFileInfoHelper();

private:
    void UpdateRecents();
    void RestoreFromRecents();
    void InitSubtitle();

    inline void SetCurWnd(FileSubType v);
    inline QString GetMuteToolTip();
    QString GetTimeFromMSec(qint64);
    QtAV::VideoRenderer *GetVideoRender();

    QtAV::AVPlayer m_controler;
    QtAV::VideoRenderer *m_pwnd_video_gl;

    bool m_is_new_playing;

    const QTime m_const_time;
    QSlider *m_pslider;
    QLabel *m_pwnd_audio;
    QLabel *m_plabel_time;
    QToolButton *m_pbtn_play;
    QStackedWidget *m_pwnd_stack;

private slots:
    void SlotMediaStarted();
    void SlotPlayBtnTriggered(bool c);
    void SlotPlayingStopped();
    void SlotCtrlPosChanged(qint64 position);
    void SlotPlayerErr(const QtAV::AVError &e);
};
#endif

#endif  // CMEDIAWND_H
