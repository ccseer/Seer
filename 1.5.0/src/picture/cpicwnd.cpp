#include "cpicwnd.h"
#include "cglobaldata.h"
#include "cpicturemanager.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "utilities.h"

#include <QKeyEvent>
#include <QMovie>
#include <QTime>
#include "qaction.h"

CPicWnd::CPicWnd(QWidget *parent)
    : CAbstractWnd(ManagerPtr(new CPictureManager), parent)
{
    SetWnd(&m_viewer);
    m_viewer.scene()->setBackgroundBrush(g_pstyle->GetSceneBackgroundColor());
    connect((CPictureManager *)m_manager.data(),
            &CPictureManager::SigFullImageReady, &m_viewer,
            &CImageView::SlotFullImageReady);

    connect((CPictureManager *)m_manager.data(),
            &CPictureManager::SigFullImageReady, this, [=](const QImage &pix) {
                if (!pix.isNull()) {
                    return;
                }
                SetActionVisible(m_pactions.value(CImageView::SizeScale),
                                 false);
            });
}

void CPicWnd::InitControlWnd(QHBoxLayout *lay)
{
    lay->addStretch();
    if (g_data->path == g_path_help) {
        return;
    }
    auto lam = [=](fa::icon p1, const QString &tooltip, const QString &k,
                   CImageView::ActionType act) {
        auto btn = GetBottomBtn(p1, tooltip, k, m_viewer.GetAction(act));
        this->addAction(btn->defaultAction());
        lay->addWidget(btn);
        btn->setVisible(false);
        m_pactions.insert(act, btn);
    };

    if (g_data->sub_type == S_Movie) {
        QToolButton *btn
            = GetBottomBtn(fa::ctrl_play, tr("Play / Pause"),
                           g_pini->GetIni(Ini::key_media_play_str).toString());
        btn->defaultAction()->setCheckable(true);
        btn->defaultAction()->setChecked(true);
        this->addAction(btn->defaultAction());
        lay->addWidget(btn);
        connect(btn->defaultAction(), &QAction::triggered, &m_viewer,
                &CImageView::SetMoviePlay);
        connect(&m_viewer, &CImageView::SigMovieStateChanged,
                [=](QMovie::MovieState state) {
                    if (QMovie::Running == state
                        && !btn->defaultAction()->isChecked()) {
                        btn->defaultAction()->setChecked(true);
                    }
                    else if (QMovie::Running != state
                             && btn->defaultAction()->isChecked()) {
                        btn->defaultAction()->setChecked(false);
                    }
                });

        // seek
        QAction *act = new QAction(this);
        act->setShortcut(g_pini->GetIni(Ini::key_media_forward_str).toString());
        connect(act, &QAction::triggered, &m_viewer,
                &CImageView::SeekMovieForward);
        this->addAction(act);
        act = new QAction(this);
        act->setShortcut(
            g_pini->GetIni(Ini::key_media_backward_str).toString());
        connect(act, &QAction::triggered, &m_viewer,
                &CImageView::SeekMovieBackward);
        this->addAction(act);
    }

    lam(fa::ctrl_pagewidth, tr("Actual Size"),
        g_pini->GetIni(Ini::key_img_actual_sz_str).toString(),
        CImageView::SizeScale);
    lam(fa::ctrl_right, tr("Rotate Right"),
        g_pini->GetIni(Ini::key_img_rotate_r_str).toString(),
        CImageView::Rotate270);
    auto act = m_viewer.GetAction(CImageView::Rotate90);
    act->setShortcut(g_pini->GetIni(Ini::key_img_rotate_l_str).toString());
    this->addAction(act);

    QLabel *separator = new QLabel("|");
    m_resolution.setObjectName("ctrlbar_big_label");
    lay->addWidget(separator);
    lay->addWidget(&m_resolution);
}

void CPicWnd::PerformFinished(const QVariant &v)
{
    if (!GetErrMsg().isEmpty()) {
        elprint << GetErrMsg();
        emit SigErr();
        return;
    }

    auto manager = (CPictureManager *)m_manager.data();
    if (g_data->sub_type == S_Movie) {
        auto mv = v.value<QMovie *>();
        m_viewer.SetImage(mv);
        m_sz_preview = manager->GetImgFullSz();
    }
    else {
        auto pic = v.value<QImage>();
        m_viewer.SetImage(pic);
        // preview size
        m_sz_preview = pic.size();
        if (m_sz_preview != manager->GetImgFullSz()) {
            SetActionVisible(m_pactions.value(CImageView::SizeScale), true);
            if (auto btn = m_pactions.value(CImageView::SizeScale)) {
                btn->defaultAction()->setChecked(false);
            }
        }
    }

    SetActionVisible(m_pactions.value(CImageView::Rotate270), true);

    const auto sz_full = manager->GetImgFullSz();
    m_resolution.setText(
        QString("%1×%2").arg(sz_full.width()).arg(sz_full.height()));

    emit SigLoaded();
}

void CPicWnd::keyPressEvent(QKeyEvent *e)
{
    if (e->matches(QKeySequence::Copy)) {
        m_viewer.CopyContent2Clipboard();
    }
    CAbstractWnd::keyPressEvent(e);
}

QSize CPicWnd::GetSize()
{
    switch (g_data->sub_type) {
        case S_Unknown:
            return GetMinWndSz(m_dpi->ratio());
        default:
            return m_sz_preview;
    }
}

void CPicWnd::SetActionVisible(QToolButton *act, bool ok)
{
    if (act) {
        act->setVisible(ok);
    }
}

FilePropertyData CPicWnd::LoadFileInfoHelper()
{
    FilePropertyData res;

    const auto sz_img = ((CPictureManager *)m_manager.data())->GetImgFullSz();
    res.append(FileProperty{tr("Width"), QString::number(sz_img.width())});
    res.append(FileProperty{tr("Height"), QString::number(sz_img.height())});

    if (g_data->sub_type == S_Movie) {
        QImageReader mv(g_data->path);
        if (mv.canRead()) {
            res.append(FileProperty{tr("Image Count"),
                                    QString::number(mv.imageCount())});
            const auto lc = mv.loopCount();
            res.append(FileProperty{
                tr("Loop Count"), lc == -1 ? "Forever" : QString::number(lc)});
        }
    }

    const QFileInfo info(g_data->path);
    if (info.size() < g_img_max_read_sz
        && (info.suffix().toLower() == "jpg"
            || info.suffix().toLower() == "jpeg")) {
        const QMap<QString, QString> exif = GetExifInfo(g_data->path);
        auto iter                         = exif.begin();
        while (iter != exif.end()) {
            if (!iter.value().isEmpty()) {
                res.append(FileProperty{iter.key(), iter.value()});
            }
            ++iter;
        }
    }
    return res;
}
