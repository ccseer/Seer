#include "csettingswnd4key.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4key.h"

#include "qlineedit.h"
#include "qscrollbar.h"
#include "qtimer.h"

#define g_invalid QT_TRANSLATE_NOOP("CSettingsWnd4Key", "Invalid")

CSettingsWnd4Key::CSettingsWnd4Key(QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4Key)
{
    ui->setupUi(this);

    ui->scrollArea->verticalScrollBar()->setObjectName("sub");

    // ui content
    ui->tabWidget->setCurrentWidget(ui->tab_common);

    InitHookKey();
    InitSC();
    ui->tabWidget->setCurrentIndex(0);

    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("line") && !i->inherits("QLineEdit")) {
            i->setObjectName("line");
        }
        else if (i->objectName().startsWith("label_group")) {
            i->setObjectName("label_group");
        }
        else if (i->objectName().contains("fixed_width_label")
                 && i->inherits("QLabel")) {
            i->setObjectName("fixed_width_label");
        }
    }
    ui->label_close->setObjectName("colored_label");
    ui->label_full->setObjectName("colored_label");
    ui->label_key_res_fullscreen->setObjectName("colored_label");
    ui->label_normal->setObjectName("colored_label");
    ui->label_key_res_normal->setObjectName("colored_label");
}

CSettingsWnd4Key::~CSettingsWnd4Key()
{
    delete ui;
}

void CSettingsWnd4Key::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    // ui display
    const auto x = dpi->x();
    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("label_key_spacer")) {
            i->setFixedWidth(x * 2);
        }
    }
    ui->Layout_scroll_content->setSpacing(x * 8);
    ui->Layout_widget_title->setContentsMargins(0, 0, 0, 3 * x);
    ui->Layout_trigger_key_container->setSpacing(5 * x);

    auto f = ui->label_full->font();
    f.setPixelSize(13 * dpi->ratio());
    ui->label_full->setFont(f);
    ui->label_normal->setFont(f);
    auto max = qMax(ui->label_full->width(), ui->label_normal->width());
    ui->label_full->setFixedWidth(max);
    ui->label_normal->setFixedWidth(max);
    ui->label_key_res_normal->setFont(f);
    ui->label_key_res_fullscreen->setFont(f);

    // label
    int max_len = 0;
    foreach (auto i, this->findChildren<QLabel *>()) {
        if (i->objectName().contains("fixed_width_label")) {
            i->setFont(f);
            max_len = qMax(max_len, i->fontMetrics().width(i->text()));
        }
    }
    max_len += x * 4;
    foreach (auto i, this->findChildren<QLabel *>()) {
        if (i->objectName().contains("fixed_width_label")) {
            i->setFixedWidth(max_len);
        }
    }

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
    ui->Layout_container->setContentsMargins(9 * x, title_height - height / 2,
                                             9 * x, 0);
}

void CSettingsWnd4Key::InitHookKey()
{
    if (g_pini->GetIni(Ini::use_hook_key_close_b).toBool()) {
        QString res;
        if (g_pini->GetIni(Ini::key_ctrl_b).toBool()) {
            res.append("Ctrl+");
        }
        if (g_pini->GetIni(Ini::key_shift_b).toBool()) {
            res.append("Shift+");
        }
        if (g_pini->GetIni(Ini::key_alt_b).toBool()) {
            res.append("Alt+");
        }
        res.append("Space");

        ui->label_close->setText(ui->label_close->text() + " / " + res);
    }

    ui->label_group_trigger_key->setText(tr("Trigger Keys"));
    ui->label_key_res_normal->setText("Space");
    ui->label_key_res_fullscreen->setText("Space");

    // normal
    foreach (auto i, ui->widget_key->findChildren<QCheckBox *>()) {
        connect(i, &QCheckBox::clicked, [=]() {
            QString text;
            if (ui->checkBox_trigger_c->isChecked()) {
                text.append("Ctrl+");
            }
            if (ui->checkBox_trigger_s->isChecked()) {
                text.append("Shift+");
            }
            if (ui->checkBox_trigger_a->isChecked()) {
                text.append("Alt+");
            }
            text.append("Space");

            if (text == ui->label_key_res_fullscreen->text()) {
                ui->label_key_res_fullscreen->setText(
                    qApp->translate("CSettingsWnd4Key", g_invalid));
            }

            ui->label_key_res_normal->setText(text);
        });
    }
    if (g_pini->GetIni(Ini::key_alt_b).toBool()) {
        ui->checkBox_trigger_a->click();
    }
    if (g_pini->GetIni(Ini::key_ctrl_b).toBool()) {
        ui->checkBox_trigger_c->click();
    }
    if (g_pini->GetIni(Ini::key_shift_b).toBool()) {
        ui->checkBox_trigger_s->click();
    }

    // full screen
    foreach (auto i, ui->widget_full->findChildren<QCheckBox *>()) {
        connect(i, &QCheckBox::clicked, [=]() {
            QString text;
            if (ui->checkBox_trigger_c_f->isChecked()) {
                text.append("Ctrl+");
            }
            if (ui->checkBox_trigger_s_f->isChecked()) {
                text.append("Shift+");
            }
            if (ui->checkBox_trigger_a_f->isChecked()) {
                text.append("Alt+");
            }
            text.append("Space");
            if (text == ui->label_key_res_normal->text()) {
                text = qApp->translate("CSettingsWnd4Key", g_invalid);
            }
            ui->label_key_res_fullscreen->setText(text);
        });
    }
    if (g_pini->GetIni(Ini::key_alt_f_b).toBool()) {
        ui->checkBox_trigger_a_f->click();
    }
    if (g_pini->GetIni(Ini::key_ctrl_f_b).toBool()) {
        ui->checkBox_trigger_c_f->click();
    }
    if (g_pini->GetIni(Ini::key_shift_f_b).toBool()) {
        ui->checkBox_trigger_s_f->click();
    }

    if (ui->label_key_res_fullscreen->text()
        == ui->label_key_res_normal->text()) {
        ui->label_key_res_fullscreen->setText(
            qApp->translate("CSettingsWnd4Key", g_invalid));
    }
}

void CSettingsWnd4Key::InitSC()
{
    // object name -> key_media_go_start_str
    // key name -> media_go_start
    const auto keys = g_pini->GetKeyGrounpVal().keys();
    foreach (auto i, this->findChildren<QKeySequenceEdit *>()) {
        auto iter = find_if(keys.begin(), keys.end(), [=](const QString &k) {
            return i->objectName().remove("keySequenceEdit_") == k;
        });

        if (iter == keys.end()) {
            elprint;
        }
        else {
            i->setKeySequence(g_pini->GetIni(*iter).toString());
            m_keys.insert(*iter, i);
            if (auto ed_key
                = i->findChild<QLineEdit *>("qt_keysequenceedit_lineedit")) {
                connect(ed_key, &QLineEdit::textChanged, [=]() {
                    const auto key_str = i->keySequence().toString();
                    if (i->keySequence().count() > 1) {
                        i->setKeySequence(QKeySequence::fromString(
                            key_str.split(", ").last()));
                    }
                    else if (key_str.contains("Backspace",
                                              Qt::CaseInsensitive)) {
                        i->setKeySequence(QKeySequence());
                    }
                    else if (key_str.contains("Space", Qt::CaseInsensitive)) {
                        emit SigShowInfoText(tr("Can not have 'Space' key."),
                                             8000);
                        i->setKeySequence(QKeySequence());
                    }
                });
            }
        }
    }
}

void CSettingsWnd4Key::WriteSettings()
{
    g_pini->SetIni(Ini::key_shift_b, ui->checkBox_trigger_s->isChecked());
    g_pini->SetIni(Ini::key_alt_b, ui->checkBox_trigger_a->isChecked());
    g_pini->SetIni(Ini::key_ctrl_b, ui->checkBox_trigger_c->isChecked());

    // 如果是一样的也写入, 但会在取值的地方不使用 full screen 的值，无法全屏激活
    g_pini->SetIni(Ini::key_shift_f_b, ui->checkBox_trigger_s_f->isChecked());
    g_pini->SetIni(Ini::key_alt_f_b, ui->checkBox_trigger_a_f->isChecked());
    g_pini->SetIni(Ini::key_ctrl_f_b, ui->checkBox_trigger_c_f->isChecked());

    auto iter = m_keys.begin();
    while (iter != m_keys.end()) {
        g_pini->SetIni(iter.key(), iter.value()->keySequence().toString());
        ++iter;
    }
}
