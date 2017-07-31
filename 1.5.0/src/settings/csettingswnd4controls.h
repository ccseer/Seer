#ifndef CSETTINGSWND4CONTROLS_H
#define CSETTINGSWND4CONTROLS_H

#include <QCompleter>
#include <QWidget>

#include "cscreenadapter.h"
#include "types.h"

class CSettingsWnd4ControlsEdit;

namespace Ui {
class CSettingsWnd4Controls;
}

class CSettingsWnd4Controls : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4Controls(QWidget *parent = 0);
    ~CSettingsWnd4Controls();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

    void WriteSettings();

private:
    CSettingsWnd4ControlsEdit *CreateContainer();

    QCompleter *m_pcompleter;

    CScreenAdapter *m_dpi;

    Ui::CSettingsWnd4Controls *ui;

signals:
    void SigShowInfoText(const QString &text, ushort time_t);
    void SigNotifyAllContainer(CSettingsWnd4ControlsEdit *);

private slots:
    void ReadSettings();

    CSettingsWnd4ControlsEdit *AddNewContainer();

    void on_pushButton_about_ctrls_clicked();
};

#endif  // CSETTINGSWND4CONTROLS_H
