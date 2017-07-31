#include "cabstractwnd.h"
#include "ccomponent.h"
#include "cfileproperty.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "utilities.h"

#include <QAction>
#include <QFileIconProvider>
#include <QLabel>
#include <QProcess>
#include "qgridlayout.h"
#include "qmenu.h"

CAbstractWnd::CAbstractWnd(const ManagerPtr &manager, QWidget *parent)
    : QWidget(parent), m_manager(manager), m_playout(new QVBoxLayout)
{
    connect(this, &CAbstractWnd::SigLoaded, [=]() {
        qprint << "Time elapsed:" << m_t_debug.elapsed() << "ms,"
               << g_data->path;
    });
}

CAbstractWnd::~CAbstractWnd() {}

void CAbstractWnd::Init(CMonitorChecker *dpi)
{
    m_dpi = dpi;
    connect(m_dpi, &CMonitorChecker::ratioChanged, this, [=](qreal r) {
        auto f = this->font();
        f.setPixelSize(9 * r);
        this->setFont(f);
        UpdateDPI(r);
    });

    this->setLayout(m_playout);
    m_playout->setSpacing(0);
    m_playout->setContentsMargins(0, 0, 0, 0);

    // can't connect a virtual function in construct func
    connect(m_manager.data(), &CAbstractManager::SigPerformFinished, this,
            &CAbstractWnd::PerformFinished);
}

void CAbstractWnd::InitControlWnd(QHBoxLayout *lay)
{
    lay->addStretch();
}

void CAbstractWnd::Load()
{
    m_manager->SetErrMsg("");
    m_t_debug.start();

    LoadHelper();
}

QSize CAbstractWnd::GetSize()
{
    return GetMinWndSz(m_dpi->ratio());
}

void CAbstractWnd::SetWnd(QWidget *wnd)
{
    m_playout->insertWidget(0, wnd);
}

QToolButton *CAbstractWnd::GetBottomBtn(const QPixmap &normal,
                                        const QPixmap &active,
                                        const QString &tooltip,
                                        const QString &key /*= ""*/,
                                        QAction *act /*= NULL*/)
{
    if (!act) {
        act = new QAction(this);
    }

    QIcon icon;
    icon.addPixmap(normal, QIcon::Normal, QIcon::Off);
    icon.addPixmap(active, QIcon::Normal, QIcon::On);
    act->setIcon(icon);
    act->setToolTip(tooltip);
    if (!key.isEmpty()) {
        act->setShortcut(QKeySequence(key));
    }

    auto btn = new QToolButton(this);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setDefaultAction(act);
    return btn;
}

QToolButton *CAbstractWnd::GetBottomBtn(const fa::icon &text,
                                        const QString &tooltip,
                                        const QString &key,
                                        QAction *act)
{
    auto btn = GetBottomBtn({}, {}, tooltip, key, act);
    auto def_act = btn->defaultAction();
    def_act->setIcon({});
    def_act->setText(fa::c(text));
    return btn;
}

void CAbstractWnd::LoadHelper()
{
    m_manager->PerformerPrivate();
}

void CAbstractWnd::LoadFileInfo()
{
    g_pfile_property->AppendFileProperty(LoadFileInfoHelper());
}

void CAbstractWnd::SetErrMsg(const QString &err)
{
    m_manager->SetErrMsg(err);
}

QString CAbstractWnd::GetErrMsg() const
{
    return m_manager->GetErrMsg();
}
