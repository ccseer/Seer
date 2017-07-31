#include "cmediawnd.h"
#include "ccomponent.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "utilities.h"

#ifdef MEDIA
#include <QMouseEvent>
#include "QClipboard"
#include "qaction.h"
#include "qapplication.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qslider.h"
#include "qstackedwidget.h"
#include "qstyle.h"

CMediaWnd::CMediaWnd(QWidget *parent)
    : CAbstractWnd(ManagerPtr(new CAbstractManager), parent),
      m_pwnd_video_gl(NULL),
      m_is_new_playing(false),
      m_const_time(0, 0, 0),
      m_pwnd_audio(new QLabel),
      m_plabel_time(new QLabel),
      m_pwnd_stack(new QStackedWidget)
{
    m_pwnd_audio->setAlignment(Qt::AlignCenter);
    m_pwnd_audio->setText(fa::c(fa::music));
    m_pwnd_audio->setObjectName("AudioPix");
    m_pwnd_stack->addWidget(m_pwnd_audio);
    m_pwnd_stack->addWidget(GetVideoRender()->widget());
    SetWnd(m_pwnd_stack);

    m_controler.setRenderer(GetVideoRender());
    m_controler.audio()->setMute(g_pini->GetIni(Ini::media_mute_b).toBool());
    m_controler.videoCapture()->setAutoSave(false);
    m_controler.videoCapture()->setAsync(true);
    m_controler.setRepeat(0);
    connect(m_controler.videoCapture(), &QtAV::VideoCapture::imageCaptured,
            this,
            [=](const QImage &image) { qApp->clipboard()->setImage(image); });
    connect(m_controler.videoCapture(), &QtAV::VideoCapture::failed, this,
            [=]() { emit SigShowMsg(tr("Capture failed")); });
}

void CMediaWnd::UpdateRecents()
{
    if (g_data->sub_type != S_Video || g_data->path.isEmpty()) {
        return;
    }

    // QList<QVariantList>  <path,pos>
    QVariantList recents_list
        = g_pini->GetIni(Ini::media_recents_list).toList();
    foreach (const QVariant &i, recents_list) {
        const auto interal = i.toList();
        if (interal[0] == g_data->path) {
            recents_list.removeOne(interal);
            break;
        }
    }
    recents_list.prepend(QVariant::fromValue(
        QVariantList{g_data->path, m_controler.position()}));
    if (recents_list.size() > g_media_recents_max) {
        recents_list = recents_list.mid(0, g_media_recents_max);
    }

    g_pini->SetIni(Ini::media_recents_list, recents_list);
}

void CMediaWnd::RestoreFromRecents()
{
    if (g_data->sub_type != S_Video) {
        return;
    }

    foreach (const QVariant &i,
             g_pini->GetIni(Ini::media_recents_list).toList()) {
        const auto interal = i.toList();
        if (interal[0] == g_data->path) {
            m_controler.setPosition(interal[1].toLongLong());
            break;
        }
    }
}

void CMediaWnd::InitSubtitle()
{
    if (g_data->sub_type != S_Video) {
        return;
    }

    auto subtitle = new QtAV::SubtitleFilter(this);
    const QFileInfo info(g_data->path);
    auto sub_path = info.absolutePath() + "/" + info.baseName() + ".";
    foreach (const auto &i, subtitle->supportedSuffixes()) {
        const auto abs_path = sub_path + i;
        if (QFile::exists(abs_path)) {
            subtitle->installTo(m_pwnd_video_gl);
            subtitle->setFile(abs_path);
            break;
        }
    }
}

CMediaWnd::~CMediaWnd()
{
    UpdateRecents();

    m_controler.stop();
    cfree(m_pwnd_video_gl);
}

void CMediaWnd::InitControlWnd(QHBoxLayout *lay)
{
    using namespace QtAV;

    m_pbtn_play
        = GetBottomBtn(fa::ctrl_pause, tr("Play / Pause"),
                       g_pini->GetIni(Ini::key_media_play_str).toString());
    m_pbtn_play->defaultAction()->setCheckable(true);
    m_pbtn_play->defaultAction()->setChecked(true);
    m_pbtn_play->setObjectName("media_play_btn");
    this->addAction(m_pbtn_play->defaultAction());

    auto btn_mute
        = GetBottomBtn(fa::ctrl_mute, GetMuteToolTip(),
                       g_pini->GetIni(Ini::key_media_mute_str).toString());
    btn_mute->defaultAction()->setCheckable(true);
    btn_mute->defaultAction()->setChecked(
        g_pini->GetIni(Ini::media_mute_b).toBool());
    this->addAction(btn_mute->defaultAction());

    m_pslider = new QSlider(Qt::Horizontal);
    m_pslider->installEventFilter(this);

    lay->addWidget(m_pbtn_play);
    lay->addWidget(m_pslider);
    lay->addWidget(m_plabel_time);
    lay->addWidget(btn_mute);

    m_plabel_time->setVisible(g_pini->GetIni(Ini::media_time_b).toBool());

    connect(btn_mute->defaultAction(), &QAction::triggered, [=](bool c) {
        if (auto au = m_controler.audio()) {
            au->setMute(c);
        }
        g_pini->SetIni(Ini::media_mute_b, c);
        btn_mute->defaultAction()->setToolTip(GetMuteToolTip());
    });

    connect(m_pbtn_play->defaultAction(), &QAction::triggered, this,
            &CMediaWnd::SlotPlayBtnTriggered);
    connect(&m_controler, &QtAV::AVPlayer::stopped, this,
            &CMediaWnd::SlotPlayingStopped);
    connect(&m_controler, &QtAV::AVPlayer::started, this,
            &CMediaWnd::SlotMediaStarted);
    connect(&m_controler, &QtAV::AVPlayer::error, this,
            &CMediaWnd::SlotPlayerErr);

    // slider
    connect(&m_controler, &AVPlayer::positionChanged, this,
            &CMediaWnd::SlotCtrlPosChanged);
    connect(m_pslider, &QSlider::sliderReleased, [=]() {
        m_controler.setPosition(m_pslider->value());
        // automatically play
        if (m_controler.isPlaying()) {
            m_controler.pause(false);
            m_pbtn_play->defaultAction()->setChecked(true);
        }
    });

    // action
    // seek
    auto lam = [=](const QString &ini) {
        auto key     = g_pini->GetIni(ini).toString();
        QAction *act = NULL;
        if (!key.isEmpty()) {
            act = new QAction(this);
            act->setShortcut(key);
            this->addAction(act);
        }
        return act;
    };
    if (auto act = lam(Ini::key_media_forward_str)) {
        connect(act, &QAction::triggered, &m_controler,
                &QtAV::AVPlayer::seekForward);
    }
    if (auto act = lam(Ini::key_media_backward_str)) {
        connect(act, &QAction::triggered, &m_controler,
                &QtAV::AVPlayer::seekBackward);
    }
    if (auto act = lam(Ini::key_media_time_indication_str)) {
        connect(act, &QAction::triggered, [=]() {
            g_pini->SetIni(Ini::media_time_b,
                           !g_pini->GetIni(Ini::media_time_b).toBool());
            m_plabel_time->setVisible(
                g_pini->GetIni(Ini::media_time_b).toBool());
        });
    }

    // loop
    if (auto act = lam(Ini::key_media_loop_str)) {
        connect(act, &QAction::triggered, [=]() {
            const auto l = !g_pini->GetIni(Ini::media_loop_b).toBool();
            g_pini->SetIni(Ini::media_loop_b, l);
            emit SigShowMsg(l ? tr("Loop: On") : tr("Loop: Off"));
        });
    }

    // Capture
    auto act = new QAction(this);
    act->setShortcut(QKeySequence::Copy);
    connect(act, &QAction::triggered, m_controler.videoCapture(),
            &QtAV::VideoCapture::capture);
    this->addAction(act);

    // Home
    if (auto act = lam(Ini::key_media_go_start_str)) {
        connect(act, &QAction::triggered, &m_controler,
                [&]() { m_controler.seek(qint64(0)); });
    }
}

FilePropertyData CMediaWnd::LoadFileInfoHelper()
{
    FilePropertyData res;

    const auto ss = m_controler.statistics();
    res.append(FileProperty{tr("Duration"), ss.duration.toString()});
    res.append(
        FileProperty{tr("Bit Rate"), QString::number(ss.audio.bit_rate)});
    res.append(FileProperty{tr("Format"), ss.format});
    res.append(FileProperty{tr("Decoder"), ss.audio.decoder});

    if (g_data->sub_type == S_Video) {
        res.append(
            FileProperty{tr("Width"), QString::number(ss.video_only.width)});
        res.append(
            FileProperty{tr("Height"), QString::number(ss.video_only.height)});
    }
    else {
        res.append(FileProperty{tr("Frame Size"),
                                QString::number(ss.audio_only.frame_size)});
        res.append(FileProperty{tr("Sample Format"), ss.audio_only.sample_fmt});
        res.append(FileProperty{tr("Sample Rate"),
                                QString::number(ss.audio_only.sample_rate)});
        res.append(FileProperty{tr("Channels"),
                                QString::number(ss.audio_only.channels)});
        res.append(
            FileProperty{tr("Channel Layout"), ss.audio_only.channel_layout});
    }
    return res;
}

void CMediaWnd::SlotCtrlPosChanged(qint64 position)
{
    if (!m_pslider->isSliderDown()) {
        m_pslider->setValue(position);
    }
    if (m_plabel_time->isVisible()) {
        m_plabel_time->setText(GetTimeFromMSec(position) + " / "
                               + GetTimeFromMSec(m_pslider->maximum()));
    }
}

QString CMediaWnd::GetTimeFromMSec(qint64 ms)
{
    auto time = m_const_time.addMSecs(ms);
    if (time.hour() == 0) {
        return time.toString("mm:ss");
    }
    return time.toString("hh:mm:ss");
}

QtAV::VideoRenderer *CMediaWnd::GetVideoRender()
{
    if (m_pwnd_video_gl == NULL) {
        m_pwnd_video_gl = new QtAV::OpenGLWidgetRenderer;
        m_pwnd_video_gl->setBackgroundColor(g_pstyle->GetMediaBgColor());
    }

    return m_pwnd_video_gl;
}

QSize CMediaWnd::GetSize()
{
    if (!m_controler.isLoaded()) {
        return GetMinWndSz(m_dpi->ratio());
    }
    const auto info = m_controler.statistics().video_only;
    return QSize(info.width, info.height);
}

// synchronous load
void CMediaWnd::LoadHelper()
{
    SetCurWnd(S_Video);

    m_is_new_playing = true;
    m_controler.play(g_data->path);
}

void CMediaWnd::SlotPlayingStopped()
{
    m_pbtn_play->defaultAction()->setChecked(false);
    m_pslider->setValue(0);
    m_pslider->setEnabled(false);
    if (g_pini->GetIni(Ini::media_loop_b).toBool()) {
        SlotPlayBtnTriggered(true);
    }
}

void CMediaWnd::SlotPlayBtnTriggered(bool c)
{
    m_controler.isPlaying() ? m_controler.pause(!c) : m_controler.play();
    m_pbtn_play->defaultAction()->setText(m_pbtn_play->isChecked()
                                              ? fa::c(fa::ctrl_pause)
                                              : fa::c(fa::ctrl_play));
}

void CMediaWnd::SlotMediaStarted()
{
    m_pslider->setRange(m_controler.mediaStartPosition(),
                        m_controler.mediaStopPosition());
    m_pslider->setValue(0);
    m_pslider->setEnabled(m_controler.isSeekable());

    m_pbtn_play->defaultAction()->setChecked(true);

    if (g_data->sub_type == S_Video) {
        auto sz = GetSize();
        GetVideoRender()->setOutAspectRatio(qreal(sz.width()) / sz.height());
    }
    else {
        const auto info = m_controler.statistics().video_only;
        if (info.height == 0 || info.width == 0) {
            SetCurWnd(S_Audio);
        }
        else {
            GetVideoRender()->setBackgroundColor(g_pstyle->GetMediaBgColor());
        }
    }

    if (m_is_new_playing) {
        m_is_new_playing = false;
        emit SigLoaded();

        InitSubtitle();
        RestoreFromRecents();
    }
}

bool CMediaWnd::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_pslider && e->type() == QEvent::MouseButtonPress
        && m_pslider->isEnabled()) {
        if (auto event = (QMouseEvent *)e) {
            if (event->button() == Qt::LeftButton) {
                const int o
                    = m_pslider->style()->pixelMetric(QStyle::PM_SliderLength)
                      - 1;
                int v = QStyle::sliderValueFromPosition(
                    m_pslider->minimum(), m_pslider->maximum(),
                    event->pos().x() - o / 2, m_pslider->width() - o, false);
                m_pslider->setValue(v);
                m_controler.setPosition(v);
                event->accept();
            }
        }
    }
    return false;
}

void CMediaWnd::SetCurWnd(FileSubType v)
{
    m_pwnd_stack->setCurrentWidget(v == S_Video ? GetVideoRender()->widget()
                                                : m_pwnd_audio);
}

QString CMediaWnd::GetMuteToolTip()
{
    return g_pini->GetIni(Ini::media_mute_b).toBool() ? tr("Mute: On")
                                                      : tr("Mute: Off");
}

void CMediaWnd::SlotPlayerErr(const QtAV::AVError &e)
{
    elprint;
    emit SigErr(QtAV::AVError(e).string());
}

#endif
