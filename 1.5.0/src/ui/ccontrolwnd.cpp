#include "ccontrolwnd.h"
#include "ccomponent.h"
#include "cfileproperty.h"
#include "cglobaldata.h"
#include "ctooltipmenu.h"
#include "settingshelper.h"
#include "stable.h"
#include "utilities.h"

#include <QFileIconProvider>
#include <QHBoxLayout>

CControlWnd::CControlWnd(QWidget *parent)
    : QWidget(parent),
      m_pseparator_const(new QLabel("|")),
      m_pseparator_ctrls(new QLabel(m_pseparator_const->text())),
      m_pbtn_reveal(new QToolButton),
      m_pbtn_default(new QToolButton),
      m_layout(new QHBoxLayout),
      m_pwnd_bg(new QWidget)
{
}

void CControlWnd::UpdateDPI(qreal r)
{
    setVisible(false);
    // top 留一个像素 给 border
    m_layout->setContentsMargins(12 * r, 1, 12 * r, 0);
    m_pwnd_bg->setFixedHeight(37 * r);
}

QList<QAction *> CControlWnd::Init()
{
    //内部 套一层widget, 否则不显示 背景
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_pwnd_bg);
    setLayout(layout);
    m_pwnd_bg->setObjectName("wnd_control");
    m_pwnd_bg->setLayout(m_layout);

    // constant btn
    m_layout->addWidget(m_pbtn_default);
    QAction *act = new QAction(this);
    act->setShortcut(
        QKeySequence(g_pini->GetIni(Ini::key_open_with_def_str).toString()));
    act->setToolTip(tr("Open With Default Application"));
    m_pbtn_default->setDefaultAction(act);

    act = new QAction(this);
    act->setShortcut(QKeySequence(
        g_pini->GetIni(Ini::key_reveal_in_explorer_str).toString()));
    QFileIconProvider icon_provider;
    QIcon icon = icon_provider.icon(QFileIconProvider::Folder);
    act->setIcon(icon);
    act->setToolTip(tr("Reveal In Explorer"));
    m_pbtn_reveal->setDefaultAction(act);
    m_layout->addWidget(m_pbtn_reveal);
    m_layout->addWidget(m_pseparator_const);
    m_layout->addWidget(m_pseparator_ctrls);

    connect(m_pbtn_default->defaultAction(), &QAction::triggered, this,
            [=]() { emit SigOpenByDefault(g_pfile_property->GetCurPath()); });
    connect(m_pbtn_reveal->defaultAction(), &QAction::triggered, this, [=]() {
        emit SigRevealInExplorer(g_pfile_property->GetCurPath());
    });

    UpdateDPI(1);

    return {m_pbtn_default->defaultAction(), m_pbtn_reveal->defaultAction()};
}

void CControlWnd::ClearControlBar()
{
    const QList<QWidget *> set{m_pseparator_const, m_pseparator_ctrls,
                               m_pbtn_default, m_pbtn_reveal};

    // spacer 不是 object
    QList<QLayoutItem *> res;
    for (int i = 0; i < m_layout->count(); ++i) {
        if (auto item = m_layout->itemAt(i)) {
            if (auto wnd = item->widget()) {
                if (!set.contains(wnd)) {
                    res.append(item);
                }
            }
            else {
                res.append(item);
            }
        }
    }
    foreach (auto i, res) {
        if (auto wnd = i->widget()) {
            m_layout->removeWidget(wnd);
            wnd->deleteLater();
        }
        else {
            m_layout->removeItem(i);
            delete i;
        }
    }
}

int CControlWnd::GetCtrlBarHeight()
{
    return m_pwnd_bg->height();
}

void CControlWnd::UpdateControlsBarData()
{
    // default
    QFileIconProvider icon_provider;
    auto p     = g_pfile_property->GetCurPath();
    bool is_ok = false;
    if (QFileInfo(p).isDir()) {
        auto icon = GetFileIcon(p, is_ok);
        if (!is_ok) {
            icon = icon_provider.icon(QFileIconProvider::Folder);
        }
        m_pbtn_default->defaultAction()->setIcon(icon);
    }
    else {
        const auto exec = GetExecFilePath(p);
        m_pbtn_default->defaultAction()->setIcon(
            exec.isEmpty() ? icon_provider.icon(QFileIconProvider::File)
                           : GetFileIcon(exec));
    }

    QDir dir_parent(p);
    dir_parent.cdUp();
    auto icon = GetFileIcon(dir_parent.absolutePath(), is_ok);
    if (!is_ok) {
        icon = icon_provider.icon(QFileIconProvider::Folder);
    }
    m_pbtn_reveal->defaultAction()->setIcon(icon);

    AddControlsApp();
}

QToolButton *CControlWnd::CreateBtn(const QString &path)
{
    QToolButton *btn = new QToolButton;
    QAction *act     = new QAction(btn);
    btn->setDefaultAction(act);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setCursor(Qt::PointingHandCursor);

    act->setIcon(GetFileIcon(path));
    return btn;
}

void CControlWnd::AddControlsApp()
{
    const CtrlsDataList apps = g_data->controls.GetExecFileList(g_data->suffix);
    m_pseparator_ctrls->setVisible(!apps.isEmpty());
    if (apps.isEmpty()) {
        return;
    }

    auto lam_append_btn = [=](const QString &path,
                              QList<QToolButton *> &btn_list) {
        QToolButton *btn = CreateBtn(path);
        btn_list.append(btn);
        m_layout->insertWidget(m_layout->indexOf(m_pseparator_ctrls), btn);
        return btn;
    };

    const bool is_need_menubtn = apps.size() > 3;
    QList<QToolButton *> ctrls_btns_grp;
    QString str_tooltip;
    foreach (const CtrlsData &i, apps) {
        str_tooltip = QFileInfo(i.exe).fileName() + " " + i.args;

        QAction *act = NULL;
        if (!is_need_menubtn) {
            act = lam_append_btn(i.exe, ctrls_btns_grp)->defaultAction();
        }
        else {
            const auto btn_t = ctrls_btns_grp.size();
            if (btn_t < 2) {
                act = lam_append_btn(i.exe, ctrls_btns_grp)->defaultAction();
            }
            // add the 3rd btn -- menu btn
            else if (btn_t == 2) {
                QToolButton *menubtn = new QToolButton;
                menubtn->setObjectName("control_app_btn");
                menubtn->setText(fa::c(fa::drop_down));
                menubtn->setFocusPolicy(Qt::NoFocus);
                menubtn->setCursor(Qt::PointingHandCursor);
                menubtn->setToolTip(tr("More"));
                m_layout->insertWidget(m_layout->indexOf(m_pseparator_ctrls),
                                       menubtn);

                CTooltipMenu *menu = new CTooltipMenu(menubtn);
                menu->setObjectName("control_app_menu");
                act = menu->addAction(GetFileIcon(i.exe),
                                      QFileInfo(i.exe).baseName());
                menubtn->setMenu(menu);
                menubtn->setPopupMode(QToolButton::MenuButtonPopup);
                ctrls_btns_grp.append(menubtn);
            }
            else {
                if (auto menubtn = (QToolButton *)ctrls_btns_grp.last()) {
                    if (auto menu = menubtn->menu()) {
                        act = menu->addAction(GetFileIcon(i.exe),
                                              QFileInfo(i.exe).baseName());
                    }
                }
            }
        }
        if (act) {
            act->setToolTip(str_tooltip);
            connect(act, &QAction::triggered, act,
                    [=]() { ExecApp(i.exe, i.args); });
        }
    }
}

void CControlWnd::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    //确定位置
    if (auto wnd = parentWidget()) {
        setGeometry(0, wnd->height() - GetCtrlBarHeight(), wnd->width(),
                    GetCtrlBarHeight());
    }

    const bool show_open
        = g_pini->GetIni(Ini::is_show_open_with_default_btn_b).toBool();
    const bool show_reveal
        = g_pini->GetIni(Ini::is_show_reveal_in_explorer_btn_b).toBool();
    m_pbtn_default->setVisible(show_open);
    m_pbtn_reveal->setVisible(show_reveal);
    if (!show_open && !show_reveal) {
        m_pseparator_const->setVisible(false);
    }
}

void CControlWnd::ExecApp(const QString &app, const QString &args)
{
    bool res = true;

    const auto path_app
        = (const wchar_t *)QDir::toNativeSeparators(app).utf16();
#ifdef Q_OS_WIN
    const QString args_replaced = QString(args).replace(
        g_plugin_input_placeholder,
        QDir::toNativeSeparators(g_pfile_property->GetCurPath()));
    const wchar_t *paras = (wchar_t *)args_replaced.utf16();
    int result = (int)ShellExecute(0, (LPCWSTR)L"open", path_app, paras, 0,
                                   SW_SHOWNORMAL);
    if (SE_ERR_ACCESSDENIED == result) {
        // Requesting elevation
        result = (int)ShellExecute(0, (LPCWSTR)L"runas", path_app, paras, 0,
                                   SW_SHOWNORMAL);
    }
    if (result <= 32) {
        elprint << "result:" << result;
        res = false;
    }
    qprint << "ControlWnd::ExecApp" << QDir::toNativeSeparators(app)
           << args_replaced;
#else
    res = QProcess::startDetached(
        app, list.isEmpty() ? QStringList() << g_pfile_property->GetCurPath()
                            : list);
#endif
    res ? SigHideMainWnd() : SigWiggleMainWnd();
}
