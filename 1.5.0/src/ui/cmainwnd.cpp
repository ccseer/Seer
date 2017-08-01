#include "cmainwnd.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "extensions.h"
#include "settingshelper.h"
#include "ui_cparentdlg.h"
#include "utilities.h"
// wnds
#include "ccodewnd.h"
#include "ccontrolwnd.h"
#include "cdirwnd.h"
#include "chtmlwnd.h"
#include "cmediawnd.h"
#include "cpdfwnd.h"
#include "cpicwnd.h"
#include "cunknownwnd.h"

#include <QGraphicsOpacityEffect>
#include <QMimeDatabase>
#include <QMovie>
#include <QToolTip>
#include "qevent.h"

#include "qt_windows.h"
#include "windowsx.h"

 
 
CMainWnd::CMainWnd(CMonitorChecker *dpi, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::CParentDlg),
      m_pwnd_stack(new QStackedWidget),
      m_pwnd_load(NULL),
      m_pwnd_unknow(NULL),
      m_pwnd_cur(NULL),
      m_dpi(dpi)
{
    ui->setupUi(this);

    ui->Layout->setContentsMargins(0, 0, 0, 0);
    ui->Layout_bg->setContentsMargins(0, 0, 0, 0);

    InitControlWnd();
    InitConstantWnd();
    InitTitleBar();
    InitShortcuts();

    // m_pwnd_control 的设置父窗口需要在上面的语句初始化完之后才能调用
    m_pwnd_control->setParent(ui->widget);
    ui->Layout_bg->removeWidget(ui->wnd_title);
    ui->wnd_title->setParent(ui->widget);
    ui->wnd_title->setVisible(false);
    ui->wnd_title->installEventFilter(this);
    m_pwnd_stack->installEventFilter(this);

    // timer
    m_mouse_tracker.setInterval(250);
    connect(&m_mouse_tracker, &QTimer::timeout, this, &CMainWnd::CheckMousePos);

    connect(&m_plugins, &CPluginsHelper::SigError, this,
            &CMainWnd::LoadUnknownFile);
    connect(&m_plugins, &CPluginsHelper::SigSucceed, this, &CMainWnd::Load);

    UpdateDPI();
}

CMainWnd::~CMainWnd()
{
    delete ui;
}

void CMainWnd::InitControlWnd()
{
    m_pwnd_control = new CControlWnd;
    this->addActions(m_pwnd_control->Init());
    m_pwnd_control->setVisible(false);
    connect(m_pwnd_control, &CControlWnd::SigHideMainWnd, this,
            &CMainWnd::HideWnd);
    connect(m_pwnd_control, &CControlWnd::SigWiggleMainWnd, this,
            &CMainWnd::SigWaggleWnd);
    connect(m_pwnd_control, &CControlWnd::SigOpenByDefault, this,
            &CMainWnd::OpenByDefault);
    connect(m_pwnd_control, &CControlWnd::SigRevealInExplorer, this,
            &CMainWnd::RevealInExplorer);
}

void CMainWnd::InitTitleBar()
{
    // Close btn
    ui->pushButton_close->setText(fa::c(fa::close));
    ui->pushButton_close->setToolTip(tr("Close"));
    connect(ui->pushButton_close, &QPushButton::clicked, this,
            &CMainWnd::HideWnd);

    // Properties btn
    QAction *act = new QAction(this);
    act->setCheckable(true);
    act->setToolTip(tr("Properties"));
    act->setText(fa::c(fa::info));

    m_pbtn_info = new QToolButton;
    m_pbtn_info->setObjectName("info_btn");
    m_pbtn_info->setFocusPolicy(Qt::NoFocus);
    m_pbtn_info->setDefaultAction(act);
    ui->Layout_title_right_btns->insertWidget(
        ui->Layout_title_right_btns->indexOf(ui->pushButton_close),
        m_pbtn_info);
    // TODO: enchance, 不居中
    m_pspacer_infobtn = new QLabel;
    m_pspacer_infobtn->setObjectName("info_spacer");
    ui->Layout_title_left->insertWidget(0, m_pspacer_infobtn);
    m_pbtn_info->installEventFilter(this);

    connect(act, &QAction::triggered, this, [=](bool arg) {
        if (!arg) {
            g_pfile_property->Hide();
            return;
        }
        if (IsAvailableContentView()
            && m_pwnd_stack->currentWidget() != m_pwnd_unknow) {
            g_pfile_property->Show(QRect(mapToGlobal({0, 0}), size()),
                                   m_dpi->ratio());
        }
        else {
            act->setChecked(false);
        }
    });
    connect(g_pfile_property, &CFileProperty::SigViewShow, act,
            &QAction::setChecked);
    connect(g_pfile_property, &CFileProperty::SigPropertyReady, &m_plugins,
            &CPluginsHelper::Md5Ready);
}

void CMainWnd::DelContentWnd()
{
    for (int i = 0; i < m_pwnd_stack->count(); ++i) {
        auto wnd = m_pwnd_stack->widget(i);
        if (wnd != m_pwnd_load && wnd != m_pwnd_unknow) {
            m_pwnd_control->ClearControlBar();
            cfree(wnd);
        }
    }
    m_pwnd_cur = NULL;
}

void CMainWnd::InitConstantWnd()
{
    m_pwnd_unknow = GetContentWnd(None);
    m_pwnd_cur    = NULL;

    {
        m_pwnd_load = new QLabel(this);
        m_pwnd_load->installEventFilter(this);
        auto m = new QMovie(this);
        m->setFileName(g_pstyle->GetAppLoadingPath());
        m->setSpeed(140);
        m_pwnd_load->setMovie(m);
        m_pwnd_load->setAlignment(Qt::AlignCenter);
        m_pwnd_stack->addWidget(m_pwnd_load);
    }

    ui->Layout_child->addWidget(m_pwnd_stack);
    m_pwnd_stack->setObjectName("MainContainer");
}

CAbstractWnd *CMainWnd::GetContentWnd(DisplayType t)
{
    if (m_pwnd_cur && m_pwnd_cur->Type() == t && t != None) {
        return m_pwnd_cur;
    }

    CAbstractWnd *wnd = NULL;
    switch (t) {
        case Pic:
            wnd = new CPicWnd;
            break;
#ifdef MEDIA
        case Media:
            wnd = new CMediaWnd;
            break;
#endif
        case Dir:
            wnd = new CDirWnd;
            break;
#ifdef PDF
        case Pdf:
            wnd = new CPdfWnd;
            break;
#endif
#ifdef HIGHLIGHT
        case Code:
            wnd = new CCodeWnd;
            break;
#endif
        case Html:
            wnd = new CHtmlWnd;
            break;
        case None:
            if (!m_pwnd_unknow) {
                wnd = new CUnknownWnd;
            }
            break;
    }

    if (!wnd) {
        m_pwnd_control->ClearControlBar();
        return m_pwnd_unknow;
    }
    m_pwnd_cur = wnd;

    connect(wnd, &CAbstractWnd::SigLoaded, this, &CMainWnd::SlotLoaded);
    connect(wnd, &CAbstractWnd::SigOpenByDefault, this,
            &CMainWnd::OpenByDefault);
    connect(wnd, &CAbstractWnd::SigRevealInExplorer, this,
            &CMainWnd::RevealInExplorer);
    connect(wnd, &CAbstractWnd::SigErr, this, &CMainWnd::LoadUnknownFile);
    connect(wnd, &CAbstractWnd::SigLoadNewPath, this, &CMainWnd::Load);
    connect(wnd, &CAbstractWnd::SigHideMainWnd, this, &CMainWnd::HideWnd);
    connect(wnd, &CAbstractWnd::SigWiggleMainWnd, this,
            &CMainWnd::SigWaggleWnd);
    connect(wnd, &CAbstractWnd::SigShowMsg, this, &CMainWnd::ShowToast);

    wnd->Init(m_dpi);
    m_pwnd_control->ClearControlBar();
    wnd->InitControlWnd(m_pwnd_control->GetBtnsLayout());
    foreach (auto btn, m_pwnd_control->findChildren<QToolButton *>()) {
        btn->setCursor(Qt::PointingHandCursor);
    }
    m_pwnd_stack->addWidget(wnd);

    return wnd;
}

void CMainWnd::InitShortcuts()
{
    auto lam_shortcuts = [=](QAction *act, const QString &str) {
        act->setShortcut(QKeySequence(g_pini->GetIni(str).toString()));
        connect(this, &CMainWnd::UpdateShortcuts, this,
                [=]() { act->setShortcut(g_pini->GetIni(str).toString()); });
    };

    QAction *act = new QAction(this);
    act->setShortcut(QKeySequence("Ctrl+W"));
    connect(act, &QAction::triggered, this, &CMainWnd::HideWnd);
    this->addAction(act);

    act = new QAction(this);
    lam_shortcuts(act, Ini::key_maximum_str);
    connect(act, &QAction::triggered, [=]() {
        if (IsAvailableContentView()) {
            emit SigMaximizeWnd();
        }
    });
    this->addAction(act);

    act = m_pbtn_info->defaultAction();
    lam_shortcuts(act, Ini::key_property_str);
    this->addAction(act);

    act = new QAction(this);
    lam_shortcuts(act, Ini::key_reload_str);
    connect(act, &QAction::triggered, [=]() {
        if (IsAvailableContentView()) {
            const auto p = g_pfile_property->GetCurPath();
            g_data->path.clear();
            g_pfile_property->Clear();
            Load(p);
        }
    });
    this->addAction(act);

    connect(this, &CMainWnd::UpdateShortcuts, g_pfile_property,
            &CFileProperty::SlotUpdateShortcut4ViewWnd);
}

void CMainWnd::LoadLoading()
{
    DelContentWnd();
    m_pwnd_load->movie()->stop();
    m_pwnd_stack->setCurrentWidget(m_pwnd_load);
    m_pwnd_load->movie()->start();
}

void CMainWnd::LoadUnknownFile()
{
    DelContentWnd();
    g_data->type     = None;
    g_data->sub_type = S_Unknown;
    m_pwnd_stack->setCurrentWidget(m_pwnd_unknow);
    m_pwnd_unknow->Load();
}

void CMainWnd::Load(const QString &p)
{
    m_pbtn_info->setVisible(false);
    m_pwnd_control->setVisible(false);

    const QFileInfo info(p);
    const auto abs_path = info.absoluteFilePath();
    //在同一个文件上触发了两次即隐藏
    if (abs_path == g_data->path || IsSameSourceFile(abs_path)) {
        HideWnd();
        return;
    }

    emit SigBe4Load();

    DetectFileType(info);
    LoadLoading();

    // check once for plug-in
    if (g_data->path != g_path_help
        && !m_plugins.IsFileFromPluginConverted(abs_path)) {
        // 仅执行一次 , 因为 wnd_title 大小会根据窗口大小一起变化
        // title width         -              left+tight   margin            -
        // info+close btn with  * 2
        static auto s_title_width = [=]() {
            bool need_hide = false;
            if (ui->wnd_title->isHidden()) {
                need_hide = true;
                ui->wnd_title->setVisible(true);
            }
            int res = ui->wnd_title->width()
                      - ui->wnd_title->contentsMargins().left() * 2
                      - ui->pushButton_close->width() * 5
                      - ui->Layout_title_left->spacing() * 2
                      - ui->Layout_title_right_btns->spacing() * 2;
            if (need_hide) {
                ui->wnd_title->setVisible(false);
            }
            return res;
        }();

        // in case the filename is too loooooooooong
        auto stre = ui->label_title_text->fontMetrics().elidedText(
            info.fileName(), Qt::ElideMiddle, s_title_width);

        ui->label_title_text->setText(stre);
        this->setWindowTitle(stre);

        const auto is_plugin = m_plugins.LoadPlugin(g_data->path);
        g_pfile_property->Load(g_data->path);
        // load plugin 失败再往下走普通流程
        if (is_plugin) {
            return;
        }
    }
    GetContentWnd(g_data->type)->Load();
}

void CMainWnd::SlotLoaded()
{
    if (auto wnd = (CAbstractWnd *)sender()) {
        m_pwnd_stack->setCurrentWidget(wnd);

        m_pbtn_info->setVisible((wnd != m_pwnd_unknow)
                                && (g_data->path != g_path_help));

        wnd->LoadFileInfo();
        m_pwnd_control->UpdateControlsBarData();

        if (!g_data->is_resized_by_user) {
            emit SigAfterLoaded(wnd->GetSize());
        }

        update();
    }
}

void CMainWnd::DetectFileType(const QFileInfo &info)
{
    g_data->path     = info.absoluteFilePath();
    g_data->suffix   = info.suffix().toLower();
    g_data->type     = g_data->extensions.GetType(g_data->path);
    g_data->sub_type = g_data->extensions.GetSubType(g_data->path);

    if (g_data->sub_type == S_Folder
        && !g_pini->GetIni(Ini::is_dir_load_contens_b).toBool()) {
        g_data->type     = None;
        g_data->sub_type = S_Unknown;
        return;
    }

    if (!g_pini->GetIni(Ini::detect_type_b).toBool()) {
        return;
    }

    static QMimeDatabase db;
    // no extension file
    if (g_data->suffix.isEmpty() || g_data->type == DisplayType::None) {
        QMimeType mime = db.mimeTypeForFile(g_data->path);
        if (mime.isValid() && !mime.suffixes().isEmpty()) {
            if (mime.inherits("text/plain")) {
                g_data->suffix = "txt";
            }
            else {
                g_data->suffix = mime.suffixes().at(0).toLower();
            }
            g_data->type = g_data->extensions.GetType(g_data->path + "."
                                                      + g_data->suffix);
            g_data->sub_type = g_data->extensions.GetSubType(g_data->path + "."
                                                             + g_data->suffix);
            // qprint << "file type detecting" << g_data->suffix << g_data->type
            // << g_data->sub_type;
        }
    }
}

bool CMainWnd::IsAvailableContentView() const
{
    if (this->isVisible() && windowOpacity() == 1 && !g_data->path.isEmpty()
        && g_data->path != g_path_help
        && m_pwnd_stack->currentWidget() != m_pwnd_load) {
        return true;
    }
    return false;
}

void CMainWnd::CheckMousePos()
{
    if (m_pwnd_stack->currentWidget() == m_pwnd_load
        || g_data->path == g_path_help) {
        return;
    }
    // 按住 ctrl 后不显示
    if (bool(0xF0 & GetKeyState(VK_CONTROL))) {
        if (ui->wnd_title->isVisible()) {
            ui->wnd_title->setVisible(false);
        }
        if (m_pwnd_control->isVisible()) {
            m_pwnd_control->setVisible(false);
        }
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }
    static QPoint global_pos;
    static QPoint global_zero;
    static QRect rt_title;
    static QRect rt_bottom;
    const ushort threshold = 2 * 90 * m_dpi->ratio();

    global_pos  = QPoint{pt.x, pt.y};
    global_zero = m_pwnd_stack->mapToGlobal(QPoint(0, 0));
    rt_title
        = QRect(global_zero, QSize(m_pwnd_stack->width(),
                                   ui->label_title_text->height() + threshold));

    const auto is_visible_title = ui->wnd_title->isVisible();
    if (rt_title.contains(global_pos)) {
        if (!is_visible_title) {
            ui->wnd_title->setVisible(true);
        }
        return;
    }

    rt_bottom
        = QRect(QPoint(global_zero.x(),
                       global_zero.y() + m_pwnd_stack->height()
                           - m_pwnd_control->GetCtrlBarHeight() - threshold),
                QSize(m_pwnd_stack->width(),
                      m_pwnd_control->GetCtrlBarHeight() + threshold));
    const auto is_visible_bottom = m_pwnd_control->isVisible();
    if (rt_bottom.contains(global_pos)) {
        if (!is_visible_bottom) {
            m_pwnd_control->setVisible(true);
        }
        return;
    }

    if (is_visible_title) {
        ui->wnd_title->setVisible(false);
    }
    if (is_visible_bottom) {
        m_pwnd_control->setVisible(false);
    }
}

bool CMainWnd::eventFilter(QObject *obj, QEvent *e)
{
    const auto t = e->type();
    if (obj == m_pbtn_info) {
        if (t == QEvent::Show) {
            m_pspacer_infobtn->setVisible(true);
        }
        else if (t == QEvent::Hide) {
            m_pspacer_infobtn->setVisible(false);
        }
    }
    else if (obj == ui->wnd_title) {
        if (t == QEvent::MouseButtonPress || t == QEvent::MouseButtonRelease) {
            auto ev = (QMouseEvent *)e;
            if (ev->button() == Qt::LeftButton) {
                emit SigTitleMouseEvt(t == QEvent::MouseButtonPress);
            }
        }
    }
    else if (obj == m_pwnd_load && t == QEvent::Hide) {
        m_pwnd_load->movie()->stop();
    }
    else if (obj == m_pwnd_stack && t == QEvent::Resize) {
        auto ev = (QResizeEvent *)e;
        if (ev->oldSize().height() != ev->size().height()) {
            ui->wnd_title->setVisible(false);
            m_pwnd_control->setVisible(false);
        }
        ui->wnd_title->setFixedWidth(ev->size().width());
        m_pwnd_control->setFixedWidth(ev->size().width());
    }
    return QDialog::eventFilter(obj, e);
}

void CMainWnd::keyPressEvent(QKeyEvent *e)
{
    if (e->key() != Qt::Key_Escape) {
        return QDialog::keyPressEvent(e);
    }
}

QGraphicsEffect *CMainWnd::GetShadowEff4FloatingWnd(qreal y_offset, qreal br)
{
    QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(this);
    eff->setColor(g_pstyle->GetAppTitleBarShadowColor());
    eff->setBlurRadius(br);
    eff->setXOffset(0);
    eff->setYOffset(y_offset);
    return eff;
}

void CMainWnd::ShowWnd()
{
    QTimer::singleShot(0, &m_mouse_tracker,
                       (void (QTimer::*)()) & QTimer::start);
}

void CMainWnd::UpdateDPI()
{
    auto r = m_dpi->ratio();
    ui->wnd_title->setGraphicsEffect(GetShadowEff4FloatingWnd(-5 * r, 20 * r));
    m_pbtn_info->setFont(fa::font(r * 21));
    ui->pushButton_close->setFont(fa::font(r * 21));

    m_pwnd_control->UpdateDPI(r);
}

void CMainWnd::CleanWnd()
{
    m_mouse_tracker.stop();

    g_pfile_property->Hide();
    g_pfile_property->Clear();

    DelContentWnd();

    g_data->path.clear();
    m_plugins.Clear();
}

void CMainWnd::HideWnd()
{
    emit SigHideWnd();
}

void CMainWnd::OpenByDefault(const QString &path)
{
    QString str = g_pfile_property->GetCurPath();
    if (!path.isEmpty()) {
        str = path;
    }
    if (OpenByDefaultApp(str)) {
        HideWnd();
    }
    else {
        emit SigWaggleWnd();
    }
}

void CMainWnd::RevealInExplorer(const QString &path)
{
    C::Core::DirFile::RevealInExplorer(
        path.isEmpty() ? g_pfile_property->GetCurPath() : path);
    HideWnd();
}

void CMainWnd::ShowToast(const QString &str)
{
    if (IsAvailableContentView()) {
        const auto rt  = geometry();
        const auto fm  = QFontMetrics(QToolTip::font());
        const auto pos = mapToGlobal(QPoint(0, 0))
                         + QPoint(rt.width() / 2 - fm.width(str) / 2,
                                  rt.height() / 2 - fm.height() / 2);
        QToolTip::showText(pos, str, this, {}, 5000);
    }
}

bool CMainWnd::RespondDClick(const QPoint &pt)
{
    //	if (GetContentWnd(g_data->type)->IsPosInBottomWnd(pt))
    //	{
    //		return true;
    //	}
    if (ui->wnd_title->isVisible()) {
        const auto pp = ui->wnd_title->mapFromGlobal(pt);
        if (ui->wnd_title->geometry().contains(pp)) {
            emit SigMaximizeWnd();
            return true;
        }
    }

    if (!g_pini->GetIni(Ini::openfile_by_dclick_b).toBool()) {
        return false;
    }

    OpenByDefault(g_pfile_property->GetCurPath());
    return true;
}

