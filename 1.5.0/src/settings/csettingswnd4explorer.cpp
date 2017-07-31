#include "csettingswnd4explorer.h"
#include "ccomponent.h"
#include "cfiledialog.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4explorer.h"
#include "utilities.h"

#include "qscrollbar.h"
#include "qcoreevent.h"
#include "qsettings.h"
#include "qtimer.h"

CSettingsWnd4Explorer::CSettingsWnd4Explorer(QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4Explorer)
{
    ui->setupUi(this);

    ui->scrollArea->verticalScrollBar()->setObjectName("sub");

    ui->pushButton_temp_folder->setToolTip(tr("Temp Folder"));
    ui->pushButton_tray_icon_invoke->setToolTip(
        tr("Run Settings.exe to display tray icon when it's hidden."));

    auto icon_folder = GetFileIcon(GetAppTempPath());
    ui->pushButton_temp_folder->setIcon(icon_folder);
    ui->pushButton_temp_folder->setCursor(Qt::PointingHandCursor);
    ui->pushButton_tray_icon_invoke->setIcon(icon_folder);
    ui->pushButton_tray_icon_invoke->setCursor(Qt::PointingHandCursor);
    ui->pushButton_reset->setCursor(Qt::PointingHandCursor);

    ui->pushButton_reset_cancel->setCursor(Qt::PointingHandCursor);
    ui->pushButton_reset_cancel->setText(fa::c(fa::cancel));
    ui->pushButton_reset_cancel->setToolTip(tr("Cancel"));

    ui->pushButton_reset_confirm->setToolTip(tr("Reset"));
    ui->pushButton_reset_confirm->setText(fa::c(fa::confirm));
    ui->pushButton_reset_confirm->setCursor(Qt::PointingHandCursor);

    UpdateResetBtnVisible(true);
    ReadSettings();
    ui->wnd_font->setVisible(false);

    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("line") && !i->inherits("QLineEdit")) {
            i->setObjectName("line");
        }
        else if (i->objectName().startsWith("label_group")) {
            i->setObjectName("label_group");
        }
    }
}

CSettingsWnd4Explorer::~CSettingsWnd4Explorer()
{
    delete ui;
}

void CSettingsWnd4Explorer::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    const auto x = dpi->x();

    // ui content
    auto f = fa::font(21 * dpi->ratio());
    ui->pushButton_reset_cancel->setFont(f);
    ui->pushButton_reset_confirm->setFont(f);

    ui->label_tray_spacer->setFixedWidth(3 * x);
    ui->Layout_grp_container->setSpacing(x * 8);
    // list view 上面会 空一点出来.在这里补回来
    ui->Layout_title_tray->setContentsMargins(0, 0, 0, 3 * x);
    ui->Layout_title_others->setContentsMargins(0, 0, 0, 3 * x);
    ui->Layout_others->setSpacing(5 * x);

    auto h = ui->checkBox_hide_tray_icon->fontMetrics().height();
    ui->pushButton_temp_folder->setIconSize(QSize(h, h));
    ui->pushButton_tray_icon_invoke->setIconSize(QSize(h, h));

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

    ui->label_reset->setFixedWidth(
        ui->label_reset->fontMetrics().width(ui->label_reset->text()));
}

void CSettingsWnd4Explorer::WriteSettings()
{
    g_pini->SetIni(Ini::save_temp_folder_b,
                   ui->checkBox_temp_folder->isChecked());
    g_pini->SetIni(Ini::detect_type_b,
                   !ui->checkBox_file_type_detect->isChecked());
    g_pini->SetIni(Ini::is_dir_load_contens_b,
                   !ui->checkBox_folder_load->isChecked());
    g_pini->SetIni(Ini::is_keep_wnd_sz_pos_b,
                   ui->checkBox_keep_wnd_sz_pos->isChecked());
    g_pini->SetIni(Ini::tray_icon_use_white_b,
                   ui->checkBox_use_white_icon->isChecked());
    g_pini->SetIni(Ini::tray_icon_hide_b,
                   ui->checkBox_hide_tray_icon->isChecked());

    if (!ui->fontComboBox->currentFont().family().isEmpty()) {
        // g_pini->SetIni(Ini::app_font_str,ui->fontComboBox->currentFont().family());
    }
}

void CSettingsWnd4Explorer::ReadSettings()
{
    ui->checkBox_temp_folder->setChecked(
        g_pini->GetIni(Ini::save_temp_folder_b).toBool());
    ui->checkBox_file_type_detect->setChecked(
        !g_pini->GetIni(Ini::detect_type_b).toBool());
    ui->checkBox_folder_load->setChecked(
        !g_pini->GetIni(Ini::is_dir_load_contens_b).toBool());
    ui->fontComboBox->setFont(
        QFont(g_pini->GetIni(Ini::app_font_str).toString()));
    ui->checkBox_keep_wnd_sz_pos->setChecked(
        g_pini->GetIni(Ini::is_keep_wnd_sz_pos_b).toBool());
    // icon
    ui->checkBox_use_white_icon->setChecked(
        g_pini->GetIni(Ini::tray_icon_use_white_b).toBool());
    ui->checkBox_hide_tray_icon->setChecked(
        g_pini->GetIni(Ini::tray_icon_hide_b).toBool());
    on_checkBox_hide_tray_icon_clicked(
        ui->checkBox_hide_tray_icon->isChecked());
}

void CSettingsWnd4Explorer::on_pushButton_temp_folder_clicked()
{
    if (!C::Core::DirFile::RevealInExplorer(GetAppTempPath())) {
        emit SigShowInfoText(tr("Open temp folder failed."), 7000);
    }
}

void CSettingsWnd4Explorer::UpdateResetBtnVisible(bool show)
{
    ui->pushButton_reset_cancel->setVisible(!show);
    ui->pushButton_reset_confirm->setVisible(!show);
    ui->label_reset->setVisible(!show);
    ui->pushButton_reset->setVisible(show);
}

void CSettingsWnd4Explorer::on_pushButton_reset_cancel_clicked()
{
    UpdateResetBtnVisible(true);
}

void CSettingsWnd4Explorer::on_pushButton_reset_confirm_clicked()
{
    UpdateResetBtnVisible(true);

    // save License info after Settings reseted
    const auto is_licensed = CLicenseManager::CheckFromLocal();
    qlonglong ll           = 0;
    if (!is_licensed) {
        ll = CLicenseManager::GetTimeChecker();
    }
    QSettings s;
    s.clear();
    if (!is_licensed) {
        // restore free trial time
        CLicenseManager::SetTimeChecker(ll);
    }
    else {
        // restore key and id
        CLicenseManager::GetCurID();
        g_pini->SetIni(Ini::license_key_str, CLicenseManager::GetCurLicense());
    }

    g_pini->SetIni(Ini::version_str, QString(VERSION));

    emit SigRestartImmediately();
}

void CSettingsWnd4Explorer::on_pushButton_reset_clicked()
{
    UpdateResetBtnVisible(false);
}

void CSettingsWnd4Explorer::on_checkBox_hide_tray_icon_clicked(bool checked)
{
    ui->pushButton_tray_icon_invoke->setVisible(checked);
}

void CSettingsWnd4Explorer::on_pushButton_tray_icon_invoke_clicked()
{
    const QString p = qApp->applicationDirPath() + "/Settings.exe";
    if (!C::Core::DirFile::RevealInExplorer(p)) {
        emit SigShowInfoText(tr("Reveal Settings.exe failed."), 6000);
    }
}
