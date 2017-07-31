#ifndef CSETTINGSWND4EXPLORER_H
#define CSETTINGSWND4EXPLORER_H

#include <cscreenadapter.h>
#include <QWidget>

namespace Ui {
class CSettingsWnd4Explorer;
}

class CSettingsWnd4Explorer : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4Explorer(QWidget *parent = 0);
    ~CSettingsWnd4Explorer();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

    void WriteSettings();
    void ReadSettings();

private:
    void UpdateResetBtnVisible(bool show);

    Ui::CSettingsWnd4Explorer *ui;

signals:
    void SigShowInfoText(const QString &text, ushort time_t);
    void SigRestartImmediately();

private slots:
    void on_pushButton_temp_folder_clicked();
    void on_pushButton_reset_cancel_clicked();
    void on_pushButton_reset_confirm_clicked();
    void on_pushButton_reset_clicked();
    void on_checkBox_hide_tray_icon_clicked(bool checked);
    void on_pushButton_tray_icon_invoke_clicked();
};

#endif  // CSETTINGSWND4EXPLORER_H
