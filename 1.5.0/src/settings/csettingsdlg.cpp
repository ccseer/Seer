#include "csettingsdlg.h"
#include "ccomponent.h"
#include "cconfirmdlg.h"
#include "csettingswnd4controls.h"
#include "csettingswnd4explorer.h"
#include "csettingswnd4filetype.h"
#include "csettingswnd4general.h"
#include "csettingswnd4key.h"
#include "csettingswnd4plugins.h"
#include "cthememanager.h"
#include "stable.h"
#include "ui_csettingsdlg.h"

#include <QIcon>
#include "QPushButton"
#include "qbuttongroup.h"
#include "qevent.h"

#include <functional>

#define ChagePage(checked, m_wnd, classname)              \
    if (!checked)                                         \
        return;                                           \
    if (!m_wnd) {                                         \
        qApp->setOverrideCursor(Qt::WaitCursor);          \
        m_wnd = new classname;                            \
        connect(m_wnd, &classname::SigShowInfoText, this, \
                &CSettingsDlg::ShowMsg);                  \
        ui->stackedWidget->addWidget(m_wnd);              \
        m_wnd->UpdateUI(m_title_height, m_dpi);           \
        qApp->restoreOverrideCursor();                    \
    }                                                     \
    ui->stackedWidget->setCurrentWidget(m_wnd);

CSettingsDlg::CSettingsDlg(QWidget *parent)
    : QDialog(parent),
      m_pwnd_general(NULL),
      m_pwnd_type(NULL),
      m_pwnd_plugins(NULL),
      m_pwnd_key(NULL),
      m_pwnd_explorer(NULL),
      m_pwnd_ctrls(NULL),
      ui(new Ui::CSettingsDlg)
{
    ui->setupUi(this);
}

void CSettingsDlg::SlotBtnClicked()
{
    if (auto wnd = qobject_cast<CSettingsDlgNavBtn *>(sender())) {
        // 将其他 按钮恢复
        foreach (const auto &i, m_resource) {
            if (wnd != i.first && i.first->IsChecked()) {
                i.first->SetChecked(false);
            }
        }
        //显示选中的 widget
        QMap<QWidget *, std::function<void(bool)>> ee;
#define map_insert(btn, sig) \
    ee.insert(ui->btn,       \
              std::bind(&CSettingsDlg::sig, this, std::placeholders::_1));

        map_insert(pushButton_file_type, SlotTypeClicked);
        map_insert(pushButton_plugins, SlotPluginsClicked);
        map_insert(pushButton_keyboard, SlotKeyboardClicked);
        map_insert(pushButton_explorers, SlotExplorersClicked);
        map_insert(pushButton_controls, SlotControlsClicked);
        map_insert(pushButton_general, SlotGeneralClicked);
        ee[wnd](true);
    }
}

bool CSettingsDlg::IsNeed2Restart()
{
    bool res = false;
    foreach (auto i, m_restart_counter.values()) {
        if (i) {
            res = i;
            break;
        }
    }
    return res;
}

void CSettingsDlg::SlotRestartAppNotify(uint seed, bool restart)
{
    if (sender()) {
        m_restart_counter.insert(seed, restart);
        if (auto ptr = GetDlgPtr) {
            ptr->SetConfirmBtnText(
                tr("&OK") + (IsNeed2Restart() ? " && " + tr("Restart") : ""));
        }
    }
}

CSettingsDlg::~CSettingsDlg()
{
    delete ui;
}

void CSettingsDlg::Init(CScreenAdapter *dpi)
{
    m_dpi = dpi;
    connect(dpi, &CScreenAdapter::ratioChanged, this, [=]() {
#define UPDATEDPI(wnd)                        \
    if (wnd) {                                \
        wnd->UpdateUI(m_title_height, m_dpi); \
    }
        UPDATEDPI(m_pwnd_general);
        UPDATEDPI(m_pwnd_ctrls);
        UPDATEDPI(m_pwnd_key);
        UPDATEDPI(m_pwnd_plugins);
        UPDATEDPI(m_pwnd_type);
        UPDATEDPI(m_pwnd_explorer);
    });

    // nav
    m_resource = {
        {ui->pushButton_general, {tr("General"), fa::ini_general}},
        {ui->pushButton_controls, {tr("Controls"), fa::ini_controls}},
        {ui->pushButton_keyboard, {tr("Keyboard"), fa::ini_keyboard}},
        {ui->pushButton_plugins, {tr("Plugins"), fa::ini_plugins}},
        {ui->pushButton_file_type, {tr("Type"), fa::ini_type}},
        {ui->pushButton_explorers, {tr("Advanced"), fa::ini_advanced}},
    };

    auto iter = m_resource.begin();
    while (iter != m_resource.end()) {
        auto p = *iter;
        p.first->Init(p.second);
        connect(p.first, &CSettingsDlgNavBtn::Clicked, this,
                &CSettingsDlg::SlotBtnClicked);
        ++iter;
    }

    ui->pushButton_title->SetTitle(true);
    ui->pushButton_title->Init({tr("Settings"), QPixmap("://icons/256.png")});
    connect(ui->pushButton_title, &CSettingsDlgNavBtn::Move, this,
            [=](const QPoint &p) {
                if (auto wnd_top = GetParentDlgPtr) {
                    wnd_top->move(wnd_top->pos() + p);
                }
            });

    ui->line->setObjectName("line");

    ui->pushButton_general->SetChecked(true);
    SlotGeneralClicked(true);
}

void CSettingsDlg::SlotGeneralClicked(bool arg)
{
    bool is_first = (m_pwnd_general == NULL);

    ChagePage(arg, m_pwnd_general, CSettingsWnd4General);

    if (is_first) {
        connect(m_pwnd_general, &CSettingsWnd4General::SigRequiredRestart, this,
                &CSettingsDlg::SlotRestartAppNotify);
    }
}

void CSettingsDlg::SlotTypeClicked(bool checked)
{
    ChagePage(checked, m_pwnd_type, CSettingsWnd4FileType);
}

void CSettingsDlg::SlotPluginsClicked(bool checked)
{
    ChagePage(checked, m_pwnd_plugins, CSettingsWnd4Plugins);
}

void CSettingsDlg::SlotKeyboardClicked(bool checked)
{
    ChagePage(checked, m_pwnd_key, CSettingsWnd4Key);
}

void CSettingsDlg::SlotExplorersClicked(bool checked)
{
    bool is_first = (m_pwnd_explorer == NULL);

    ChagePage(checked, m_pwnd_explorer, CSettingsWnd4Explorer);

    if (is_first) {
        connect(m_pwnd_explorer, &CSettingsWnd4Explorer::SigRestartImmediately,
                this, &CSettingsDlg::SigReopen);
    }
}

void CSettingsDlg::SlotControlsClicked(bool checked)
{
    ChagePage(checked, m_pwnd_ctrls, CSettingsWnd4Controls);
}

void CSettingsDlg::SaveSettings()
{
#define SAVEINI(ptr)          \
    if (ptr) {                \
        ptr->WriteSettings(); \
    }

    SAVEINI(m_pwnd_type);
    SAVEINI(m_pwnd_plugins);
    SAVEINI(m_pwnd_key);
    SAVEINI(m_pwnd_explorer);
    SAVEINI(m_pwnd_ctrls);
    SAVEINI(m_pwnd_general);
}

void CSettingsDlg::ShowMsg(const QString &text, ushort time_t)
{
    DisplayInfoTextWithAni(text, time_t);
}

void CSettingsDlg::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        e->ignore();
        return;
    }
    QDialog::keyPressEvent(e);
}
