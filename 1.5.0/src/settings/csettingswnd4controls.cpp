#include "csettingswnd4controls.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "csettingswnd4controlsedit.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4controls.h"
#include "utilities.h"

#include "qlineedit.h"
#include "qpropertyanimation.h"
#include "qpushbutton.h"
#include "qscrollbar.h"
#include "qsettings.h"

CSettingsWnd4Controls::CSettingsWnd4Controls(QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4Controls), m_dpi(NULL)
{
    ui->setupUi(this);

    ui->scrollArea->verticalScrollBar()->setObjectName("sub");

    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("line") && !i->inherits("QLineEdit")) {
            i->setObjectName("line");
        }
        else if (i->objectName().startsWith("label_group")) {
            i->setObjectName("label_group");
        }
    }

    ui->pushButton_about_ctrls->setText(fa::c(fa::question));
    ui->pushButton_about_ctrls->setCursor(Qt::PointingHandCursor);
    ui->pushButton_about_ctrls->setToolTip(web_controlbar);

    auto ext = g_data->extensions.GetAll();
    QStringList res;
    foreach (const auto &i, ext.values()) {
        res.append(i);
    }
    m_pcompleter = new QCompleter(res, this);
    m_pcompleter->setCompletionMode(QCompleter::PopupCompletion);
    m_pcompleter->setCaseSensitivity(Qt::CaseInsensitive);
    auto view = m_pcompleter->popup();
    view->setObjectName("settings_ctrl_completer");
    view->verticalScrollBar()->setObjectName("sub");
    ReadSettings();
}

CSettingsWnd4Controls::~CSettingsWnd4Controls()
{
    delete ui;
}

void CSettingsWnd4Controls::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    m_dpi        = dpi;
    const auto x = dpi->x();

    ui->Layout_widget_title->setContentsMargins(0, 0, 0, 3 * x);
    ui->Layout_wnd_display->setSpacing(5 * x);
    ui->Layout_all->setVerticalSpacing(8 * x);
    ui->Layout_extension_container->setVerticalSpacing(5 * x);

    ui->pushButton_about_ctrls->setFont(fa::font(24 * dpi->ratio()));

    auto height = 0;
    foreach (auto i, findChildren<QLabel *>()) {
        if (i->objectName().startsWith("label_group")) {
            auto f = i->font();
            f.setPixelSize(15 * dpi->ratio());
            i->setFont(f);
            i->setFixedWidth(i->fontMetrics().width(i->text())
                             + i->fontMetrics().xHeight() * 2);
            height = i->height();
        }
    }
    ui->Layout_all->setContentsMargins(9 * x, title_height - height / 2, 9 * x,
                                       0);

    foreach (auto i,
             ui->scrollAreaWidgetContents
                 ->findChildren<CSettingsWnd4ControlsEdit *>()) {
        i->UpdateUI(dpi);
    }
}

void CSettingsWnd4Controls::ReadSettings()
{
    ui->checkBox_open_with_def->setChecked(
        g_pini->GetIni(Ini::is_show_open_with_default_btn_b).toBool());
    ui->checkBox_reveal_in_explorer->setChecked(
        g_pini->GetIni(Ini::is_show_reveal_in_explorer_btn_b).toBool());

    foreach (const CtrlsData &i, g_data->controls.data) {
        if (CSettingsWnd4ControlsEdit *wnd = AddNewContainer()) {
            wnd->SetExtension(i.key);
            wnd->SetPath(i.exe);
            wnd->SetArgs(i.args);

            wnd->UpdateAddBtn(i.exe);
        }
    }

    AddNewContainer();
}

void CSettingsWnd4Controls::WriteSettings()
{
    QStringList res;
    QString key, path, args;
    foreach (auto i,
             ui->scrollArea->findChildren<CSettingsWnd4ControlsEdit *>()) {
        key  = i->GetExtension();
        path = i->GetPath();
        args = i->GetArgs();
        if (!key.isEmpty() && !path.isEmpty()) {
            res.append(key + g_ctrl_bar_separator + path
                       + g_ctrl_bar_separator_args + args);
        }
    }
    res.removeDuplicates();

    g_pini->SetIni(Ini::ctrls_app_list, res);
    g_data->controls = CCtrlsBarHelper();
    g_pini->SetIni(Ini::is_show_open_with_default_btn_b,
                   ui->checkBox_open_with_def->isChecked());
    g_pini->SetIni(Ini::is_show_reveal_in_explorer_btn_b,
                   ui->checkBox_reveal_in_explorer->isChecked());
}

CSettingsWnd4ControlsEdit *CSettingsWnd4Controls::CreateContainer()
{
    auto wnd = new CSettingsWnd4ControlsEdit(m_pcompleter);
    if (m_dpi) {
        wnd->UpdateUI(m_dpi);
    }
    connect(wnd, &CSettingsWnd4ControlsEdit::SigEditClicked, this,
            &CSettingsWnd4Controls::SigNotifyAllContainer);
    connect(wnd, &CSettingsWnd4ControlsEdit::SigAddNewContainer, this,
            &CSettingsWnd4Controls::AddNewContainer);
    connect(wnd, &CSettingsWnd4ControlsEdit::SigRemoveContainer, this,
            [=](CSettingsWnd4ControlsEdit *wnd) {
                delete wnd;
                AddNewContainer();
            });

    connect(this, &CSettingsWnd4Controls::SigNotifyAllContainer, wnd,
            &CSettingsWnd4ControlsEdit::HideEditor);
    return wnd;
}

CSettingsWnd4ControlsEdit *CSettingsWnd4Controls::AddNewContainer()
{
    CSettingsWnd4ControlsEdit *wnd = 0;
    //全部不为空
    const auto list = ui->scrollAreaWidgetContents
                          ->findChildren<CSettingsWnd4ControlsEdit *>();
    const bool is_none
        = none_of(list.begin(), list.end(), [](CSettingsWnd4ControlsEdit *le) {
              return (le->GetExtension().isEmpty() || le->GetPath().isEmpty());
          });

    if (is_none) {
        wnd = CreateContainer();
        ui->Layout_extension_container->addWidget(wnd);
        //??? 没有找到方法 即时 拉下滚动
        QTimer::singleShot(200, [=]() {
            if (auto src = ui->scrollArea->verticalScrollBar()) {
                QPropertyAnimation *ani
                    = new QPropertyAnimation(src, "value", this);
                ani->setEndValue(src->maximum());
                ani->start(ani->DeleteWhenStopped);
            }
        });
    }
    return wnd;
}

void CSettingsWnd4Controls::on_pushButton_about_ctrls_clicked()
{
    OpenByDefaultApp(
        QUrl(ui->pushButton_about_ctrls->toolTip(), QUrl::TolerantMode));
}
