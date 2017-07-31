#include "csettingswnd4filetype.h"
#include "ccomboboxdelegate4separator.h"
#include "ccomponent.h"
#include "cfiledialog.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "extensions.h"
#include "settingshelper.h"
#include "stable.h"
#include "ui_csettingswnd4filetype.h"

#include <qlistview.h>
#include "qscrollbar.h"
#include "qsettings.h"
#include "qtimer.h"

#define g_del_btn_str QT_TRANSLATE_NOOP("CSettingsWnd4FileType", "Remove")
#define g_del_btn_translated qApp->translate("CSettingsWnd4FileType", g_del_btn_str)

// TODO: face_nohappy.png
CSettingsWnd4FileType::CSettingsWnd4FileType(QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4FileType)
{
    ui->setupUi(this);

    ReadSettings();
    m_text_more_old = g_pini->GetIni(Ini::text_support_list).toStringList();
    ui->lineEdit_search->setClearButtonEnabled(true);
    ui->lineEdit_search->setPlaceholderText(tr("Search extension, e.g.: psd"));

    ui->widget_del->setVisible(false);

    ui->scrollArea->verticalScrollBar()->setObjectName("sub");

    auto list = new QListView(ui->comboBox_del);
    list->setItemDelegate(new CComboBoxDelegate4Separator(ui->comboBox_del));
    list->verticalScrollBar()->setObjectName("sub");
    ui->comboBox_del->setView(list);
    ui->comboBox_del->setMaxVisibleItems(4);

    ui->pushButton_del->setText(g_del_btn_translated);
    ui->pushButton_del->setObjectName("btn_type_del");

    ui->pushButton_del_over->setText(fa::c(fa::confirm));
    ui->pushButton_remove_text->setText(fa::c(fa::ini_trash));
    ui->pushButton_add_text->setText(fa::c(fa::ini_add));
    ui->pushButton_markdown_add->setText(fa::c(fa::ini_add));
    ui->pushButton_markdown_del->setText(fa::c(fa::ini_trash));
    ui->lineEdit_markdown_css->setPlaceholderText("Markdown stylesheet path");
    ui->lineEdit_markdown_css->setReadOnly(true);

    // ui display
    foreach (auto i, ui->scrollArea->findChildren<QWidget *>()) {
        if (i->objectName().startsWith("line") && !i->inherits("QLineEdit")) {
            i->setObjectName("line");
        }
        else if (i->objectName().startsWith("label_group")) {
            i->setObjectName("label_group");
        }
        else if (i->inherits("QPushButton")) {
            i->setCursor(Qt::PointingHandCursor);
        }
    }
    const QList<QLabel *> l_contents{
        ui->label_pic, ui->label_text,  ui->label_media,
        ui->label_dir, ui->label_other,
    };
    foreach (auto i, l_contents) {
        i->setObjectName("filetype_extensions");
    }

    on_lineEdit_markdown_css_textChanged(ui->lineEdit_markdown_css->text());
}

void CSettingsWnd4FileType::ReadSettings()
{
    const QMap<DisplayType, CExtensionList> &map = g_data->extensions.GetAll();
    auto code = map.value(Code);
    ui->label_text->setText(code.forDisplay());

    auto pic = map.value(Pic);
    ui->label_pic->setText(pic.forDisplay());

    auto media = map.value(Media);
    ui->label_media->setText(media.forDisplay());

    auto dir = map.value(Dir);
    ui->label_dir->setText(dir.forDisplay());

    auto other = map.value(Pdf);
    other.append(map.value(Html));
    ui->label_other->setText(other.forDisplay());

    m_label_extentions.insert(ui->label_text, code);
    m_label_extentions.insert(ui->label_pic, pic);
    m_label_extentions.insert(ui->label_media, media);
    m_label_extentions.insert(ui->label_dir, dir);
    m_label_extentions.insert(ui->label_other, other);

    foreach (auto i, m_label_extentions.keys()) {
        i->setWordWrap(true);
    }

    foreach (auto i, this->findChildren<QWidget *>()) {
        i->setFocusPolicy(Qt::NoFocus);
    }
    ui->lineEdit_search->setFocusPolicy(Qt::ClickFocus);
    ui->lineEdit_markdown_css->setText(
        g_pini->GetIni(Ini::markdown_css_path_str).toString());
    // ui->lineEdit_markdown_css->setFocusPolicy(Qt::ClickFocus);

    ui->checkBox_text_warp->setChecked(
        g_pini->GetIni(Ini::text_wrap_b).toBool());
    ui->checkBox_text_line_num->setChecked(
        g_pini->GetIni(Ini::text_line_num_b).toBool());

    ui->checkBox_media_time->setChecked(
        g_pini->GetIni(Ini::media_time_b).toBool());
    ui->checkBox_media_mute->setChecked(
        g_pini->GetIni(Ini::media_mute_b).toBool());
    ui->checkBox_media_loop->setChecked(
        g_pini->GetIni(Ini::media_loop_b).toBool());
}

void CSettingsWnd4FileType::WriteSettings()
{
    auto list = m_text_more_old + m_text_more_new;
    list.removeDuplicates();
    g_pini->SetIni(Ini::text_support_list, list);

    g_pini->SetIni(Ini::text_wrap_b, ui->checkBox_text_warp->isChecked());
    g_pini->SetIni(Ini::text_line_num_b,
                   ui->checkBox_text_line_num->isChecked());
    g_pini->SetIni(Ini::media_time_b, ui->checkBox_media_time->isChecked());
    g_pini->SetIni(Ini::media_mute_b, ui->checkBox_media_mute->isChecked());
    g_pini->SetIni(Ini::media_loop_b, ui->checkBox_media_loop->isChecked());
    g_pini->SetIni(Ini::markdown_css_path_str,
                   ui->lineEdit_markdown_css->text().trimmed());

    g_data->extensions = CExtensions{};
}

CSettingsWnd4FileType::~CSettingsWnd4FileType()
{
    delete ui;
}

void CSettingsWnd4FileType::UpdateUI(int title_height, CScreenAdapter *dpi)
{
    auto font = fa::font(21 * dpi->ratio());
    ui->pushButton_del_over->setFont(font);
    ui->pushButton_remove_text->setFont(font);
    ui->pushButton_add_text->setFont(font);
    ui->pushButton_markdown_add->setFont(font);
    ui->pushButton_markdown_del->setFont(font);

    const auto x = dpi->x();
    foreach (auto i, ui->scrollArea->findChildren<QWidget *>()) {
        if (i->objectName().startsWith("wnd_title_container")) {
            auto wnd = qobject_cast<QWidget *>(i);
            auto lay = wnd->layout();
            lay->setContentsMargins(0, 0, 0, 3 * x);
        }
    }

    ui->Layout_group_container->setSpacing(8 * x);
    ui->Layout_all_container->setVerticalSpacing(5 * x);
    ui->Layout_widget_del->setSpacing(7 * x);
    ui->Layout_widget_text_operation->setSpacing(7 * x);
    ui->Layout_widget_media_options->setSpacing(7 * x);
    foreach (auto i, ui->scrollAreaWidgetContents->findChildren<QLayout *>()) {
        if (i->objectName().startsWith("Layout_grp")) {
            if (auto lay = qobject_cast<QVBoxLayout *>(i)) {
                lay->setSpacing(x * 5);
            }
        }
    }

    foreach (auto i, ui->scrollArea->findChildren<QLabel *>()) {
        if (i->objectName().startsWith("label_group")) {
            auto f = i->font();
            f.setPixelSize(15 * dpi->ratio());
            i->setFont(f);
            i->setFixedWidth(i->fontMetrics().width(i->text())
                             + i->fontMetrics().xHeight() * 2);
        }
        else if (i->objectName().startsWith("filetype_extensions")) {
        }
    }

    auto top = title_height - ui->lineEdit_search->height() / 2;
    ui->Layout_all_container->setContentsMargins(9 * x, top, 9 * x, 0);
}

void CSettingsWnd4FileType::showEvent(QShowEvent *e)
{
    QTimer::singleShot(0, ui->lineEdit_search, SLOT(setFocus()));
    QWidget::showEvent(e);
}

void CSettingsWnd4FileType::SetControlsWndVisible(bool show)
{
    ui->widget_media_options->setVisible(show);
    ui->widget_text_options->setVisible(show);
    ui->widget_other_options->setVisible(show);
}

void CSettingsWnd4FileType::on_lineEdit_search_textChanged(const QString &arg)
{
    ui->stackedWidget->setCurrentWidget(ui->page_content);

    foreach (auto i, findChildren<QWidget *>()) {
        if (i->objectName().startsWith("groupBox"))
            i->setVisible(true);
    }

    // display controls if arg is empty, otherwise hide them
    const QString text = arg.trimmed();
    SetControlsWndVisible(text.isEmpty());
    if (text.isEmpty()) {
        foreach (auto i, m_label_extentions.keys()) {
            i->setText(m_label_extentions.value(i).forDisplay());
        }
        return;
    }

    CExtensionList list_show;
    foreach (auto i, m_label_extentions.keys()) {
        list_show.clear();
        CExtensionList ext = m_label_extentions.value(i);
        foreach (const auto &item, ext) {
            if (item.contains(text, Qt::CaseInsensitive)) {
                list_show.append(item);
            }
        }
        i->setText(list_show.forDisplay());
    }

    foreach (auto i, m_label_extentions.keys()) {
        if (i->text().isEmpty()) {
            auto p = i->parentWidget();
            while (p) {
                if (p->objectName().startsWith("groupBox")) {
                    p->setVisible(false);
                    break;
                }
                p = p->parentWidget();
            }
        }
    }

    //全部为空时 背景告知为空
    foreach (auto i, m_label_extentions.keys()) {
        if (i->isVisible()) {
            return;
        }
    }
    ui->stackedWidget->setCurrentWidget(ui->page_404);
}

/**
 * @brief markdown css
 */
void CSettingsWnd4FileType::on_lineEdit_markdown_css_textChanged(
    const QString &arg1)
{
    ui->pushButton_markdown_del->setEnabled(!arg1.trimmed().isEmpty());
    if (arg1.isEmpty()) {
        return;
    }
    if (!QFile::exists(arg1)) {
        emit SigShowInfoText(tr("Markdown css not found."), 9000);
    }
}

void CSettingsWnd4FileType::on_pushButton_markdown_add_clicked()
{
    const auto s = CFileDialog::GetOpenFilePath(
        this, tr("Select your Text format file"), "*.css");
    if (s.isEmpty()) {
        return;
    }
    ui->lineEdit_markdown_css->setText(s);
}

void CSettingsWnd4FileType::on_pushButton_markdown_del_clicked()
{
    ui->lineEdit_markdown_css->clear();
}

/**
 * @brief add & remove costom text file
 */
void CSettingsWnd4FileType::on_pushButton_add_text_clicked()
{
    const auto s = CFileDialog::GetOpenFilePath(
        this, tr("Select your Text format file"));
    if (s.isEmpty()) {
        return;
    }

    const auto suf = QFileInfo(s).suffix().toLower();
    if (suf.isEmpty()) {
        return;
    }
    foreach (const auto &i, m_label_extentions.values()) {
        if (i.contains(suf, Qt::CaseInsensitive)) {
            ui->lineEdit_search->setText(suf);
            ui->lineEdit_search->setFocus();
            ui->lineEdit_search->selectAll();
            emit SigShowInfoText("'" + suf + "'" + tr(" already exist."), 8000);
            return;
        }
    }
    m_text_more_new.append(suf);
    m_label_extentions[ui->label_text].append(suf);
    ui->label_text->setText(
        m_label_extentions.value(ui->label_text).forDisplay());
}

void CSettingsWnd4FileType::EnterDelMode(bool a)
{
    ui->widget_del->setVisible(a);
    ui->widget_text_operation->setVisible(!a);
    on_comboBox_del_currentTextChanged(ui->comboBox_del->currentText());
}

void CSettingsWnd4FileType::on_pushButton_remove_text_clicked()
{
    EnterDelMode(true);

    ui->comboBox_del->clear();
    ui->comboBox_del->addItems(m_text_more_old);
    if (!m_text_more_new.isEmpty()) {
        ui->comboBox_del->addItems(m_text_more_new);
        ui->comboBox_del->insertSeparator(m_text_more_old.size());
    }

    on_comboBox_del_currentIndexChanged(0);
}

void CSettingsWnd4FileType::on_pushButton_del_over_clicked()
{
    EnterDelMode(false);

    QStringList list_old;
    QStringList list_new;
    for (int i = 0; i < ui->comboBox_del->count(); ++i) {
        const auto cur = ui->comboBox_del->itemText(i);
        if (m_text_more_old.contains(cur, Qt::CaseInsensitive)) {
            list_old.append(cur);
        }
        else if (m_text_more_new.contains(cur, Qt::CaseInsensitive)) {
            list_new.append(cur);
        }
    }
    m_text_more_old = list_old;
    m_text_more_new = list_new;

    // 1,没有自定义文本的 text list
    const QMap<DisplayType, CExtensionList> &map = g_data->extensions.GetAll();
    auto code = map.value(Code);
    foreach (const auto &i,
             g_pini->GetIni(Ini::text_support_list).toStringList()) {
        code.removeAll(i);
    }

    // 2,添加现在的
    code.append(m_text_more_old);
    code.append(m_text_more_new);

    // 3,重新赋值
    m_label_extentions[ui->label_text] = code;
    ui->label_text->setText(
        m_label_extentions.value(ui->label_text).forDisplay());
}

void CSettingsWnd4FileType::on_pushButton_del_clicked()
{
    ui->comboBox_del->removeItem(ui->comboBox_del->currentIndex());
    // 分隔符
    if (ui->comboBox_del->currentText().isEmpty()
        && ui->comboBox_del->count()) {
        on_pushButton_del_clicked();
    }
}

void CSettingsWnd4FileType::on_comboBox_del_currentTextChanged(
    const QString &arg1)
{
    ui->pushButton_del->setText(g_del_btn_translated);
    if (!arg1.isEmpty()) {
        ui->pushButton_del->setText(QString(g_del_btn_translated) + " " + arg1);
    }
}

void CSettingsWnd4FileType::on_comboBox_del_currentIndexChanged(int /*index*/)
{
    ui->pushButton_del->setEnabled(ui->comboBox_del->count());
}
