#ifndef CSETTINGSDLG_H
#define CSETTINGSDLG_H

#include <QDialog>
#include "qmap.h"
#include "stable.h"

#include "cscreenadapter.h"
#include "csettingsdlgnavbtn.h"

class QButtonGroup;
class CSettingsWnd4General;
class CSettingsWnd4FileType;
class CSettingsWnd4Plugins;
class CSettingsWnd4Key;
class CSettingsWnd4Explorer;
class CSettingsWnd4Controls;

namespace Ui {
class CSettingsDlg;
}

class CSettingsDlg : public QDialog {
    Q_OBJECT

    DEFINE_QSS_PROPERTY(int, title_height)

public:
    explicit CSettingsDlg(QWidget *parent = 0);
    ~CSettingsDlg();
    void Init(CScreenAdapter *dpi);

    bool IsNeed2Restart();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    void SlotTypeClicked(bool arg);
    void SlotPluginsClicked(bool checked);
    void SlotKeyboardClicked(bool checked);
    void SlotExplorersClicked(bool checked);
    void SlotControlsClicked(bool checked);
    void SlotGeneralClicked(bool checked);

    CSettingsWnd4General *m_pwnd_general;
    CSettingsWnd4FileType *m_pwnd_type;
    CSettingsWnd4Plugins *m_pwnd_plugins;
    CSettingsWnd4Key *m_pwnd_key;
    CSettingsWnd4Explorer *m_pwnd_explorer;
    CSettingsWnd4Controls *m_pwnd_ctrls;

    QMap<uint, bool> m_restart_counter;

    QList<QPair<CSettingsDlgNavBtn *, QVariantList>> m_resource;

    CScreenAdapter *m_dpi;

    Ui::CSettingsDlg *ui;
signals:
    void SigReopen();

public slots:
    void SaveSettings();
    void ShowMsg(const QString &text, ushort time_t);

private slots:
    void SlotBtnClicked();
    void SlotRestartAppNotify(uint seed, bool restart);
};

#endif  // CSETTINGSDLG_H
