#include "csettingswnd4controlsedit.h"
#include "ccomponent.h"
#include "cfiledialog.h"
#include "cglobaldata.h"
#include "cthememanager.h"
#include "stable.h"
#include "ui_csettingswnd4controlsedit.h"
#include "utilities.h"

CSettingsWnd4ControlsEdit::CSettingsWnd4ControlsEdit(QCompleter *com,
                                                     QWidget *parent)
    : QWidget(parent), ui(new Ui::CSettingsWnd4ControlsEdit)
{
    ui->setupUi(this);

    const auto ts_input = QT_TRANSLATE_NOOP("CSettingsWnd4Controls",
                                            "equals triggered file path");
    ui->lineEdit_args->setToolTip(g_plugin_input_placeholder + " " + ts_input);

    ui->lineEdit_extension->setPlaceholderText(
        QT_TRANSLATE_NOOP("CSettingsWnd4Controls", "extension"));
    ui->lineEdit_extension->setCompleter(com);
    C::Ui::Funcs::SetLineeditOnlyNumChar(ui->lineEdit_extension, 26);

    ui->pushButton_add->setToolTip(QT_TRANSLATE_NOOP(
        "CSettingsWnd4Controls", "Select executable file path"));
    ui->pushButton_add->setCursor(Qt::PointingHandCursor);
    ui->pushButton_add->setText(fa::c(fa::ini_add));

    ui->pushButton_edit->setToolTip(
        QT_TRANSLATE_NOOP("CSettingsWnd4Controls", "Edit"));
    ui->pushButton_edit->setCheckable(true);
    ui->pushButton_edit->setCursor(Qt::PointingHandCursor);
    ui->pushButton_edit->setText(fa::c(fa::ini_edit));

    ui->pushButton_remove->setCursor(Qt::PointingHandCursor);
    ui->pushButton_remove->setToolTip(
        QT_TRANSLATE_NOOP("CSettingsWnd4Controls", "Remove"));
    ui->pushButton_remove->setText(fa::c(fa::ini_trash));

    ui->label_exe->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->lineEdit_command->setVisible(false);
    ui->widget_btm->setVisible(false);
    ui->widget_btm->installEventFilter(this);
}

CSettingsWnd4ControlsEdit::~CSettingsWnd4ControlsEdit()
{
    delete ui;
}

void CSettingsWnd4ControlsEdit::UpdateUI(CScreenAdapter *dpi)
{
    const auto x = dpi->x();
    const auto r = dpi->ratio();
    ui->horizontalLayout->setSpacing(x * 2);
    ui->horizontalLayout_btm->setSpacing(x * 2);
    ui->verticalLayout->setSpacing(x * 2);
    ui->lineEdit_extension->setFixedWidth(x * 60);
    auto l = ui->label_exe;
    auto f = l->font();
    f.setPixelSize(13 * r);
    l->setFont(f);
    auto wid = l->fontMetrics().width(l->text());
    l->setFixedWidth(wid + 5 * r);

    auto h = ui->lineEdit_extension->height();
    QSize sz{h, h};
    ui->pushButton_add->setFixedSize(sz);
    ui->pushButton_add->setFont(fa::font(22 * r));
    ui->pushButton_add->setIconSize(sz);
    ui->pushButton_edit->setFont(fa::font(22 * r));
    ui->pushButton_edit->setFixedSize(sz);
    ui->pushButton_remove->setFont(fa::font(22 * r));
}

void CSettingsWnd4ControlsEdit::UpdateAddBtn(const QString &path)
{
    const auto info = QFileInfo(path);
    bool is_ok;
    auto icon = GetFileIcon(path, is_ok);
    if (!is_ok) {
        ui->pushButton_add->setText(info.baseName());
        ui->pushButton_add->setIcon({});
        ui->pushButton_add->setToolTip("");
    }
    else {
        ui->pushButton_add->setText("");
        ui->pushButton_add->setIcon(icon);
        ui->pushButton_add->setToolTip(info.baseName());
    }
    ui->label_exe->setText(info.fileName());
}

void CSettingsWnd4ControlsEdit::SetPath(const QString &p)
{
    ui->lineEdit_command->setText(p);
}

QString CSettingsWnd4ControlsEdit::GetPath() const
{
    return ui->lineEdit_command->text().trimmed();
}

void CSettingsWnd4ControlsEdit::SetExtension(const QString &e)
{
    ui->lineEdit_extension->setText(e);
}

QString CSettingsWnd4ControlsEdit::GetExtension() const
{
    return ui->lineEdit_extension->text().trimmed();
}

void CSettingsWnd4ControlsEdit::SetArgs(const QString &e)
{
    ui->lineEdit_args->setText(e);
}

QString CSettingsWnd4ControlsEdit::GetArgs() const
{
    return ui->lineEdit_args->text().trimmed();
}

bool CSettingsWnd4ControlsEdit::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_btm) {
        const auto t          = e->type();
        const auto is_checked = ui->pushButton_edit->isChecked();
        if (t == QEvent::Show && !is_checked) {
            ui->pushButton_edit->setChecked(true);
        }
        else if (t == QEvent::Hide && is_checked) {
            ui->pushButton_edit->setChecked(false);
        }
    }
    return false;
}

void CSettingsWnd4ControlsEdit::HideEditor(CSettingsWnd4ControlsEdit *editor)
{
    if (editor == this) {
        ui->widget_btm->setVisible(!ui->widget_btm->isVisible());
    }
    else {
        ui->widget_btm->setVisible(false);
    }
}

void CSettingsWnd4ControlsEdit::on_pushButton_edit_clicked(bool checked)
{
    Q_UNUSED(checked);
    emit SigEditClicked(this);
}

void CSettingsWnd4ControlsEdit::on_pushButton_add_clicked()
{
    const auto str = CFileDialog::GetOpenFilePath(
        this, QT_TRANSLATE_NOOP("CSettingsWnd4Controls", "Choose Application"),
        "Executable file(*.*)");
    if (str.isEmpty()) {
        return;
    }

    UpdateAddBtn(str);
    ui->lineEdit_command->setText(str);
    emit SigAddNewContainer();
}

void CSettingsWnd4ControlsEdit::on_pushButton_remove_clicked()
{
    emit SigRemoveContainer(this);
}

void CSettingsWnd4ControlsEdit::on_lineEdit_extension_textChanged(
    const QString &arg1)
{
    Q_UNUSED(arg1);
    emit SigAddNewContainer();
}

void CSettingsWnd4ControlsEdit::on_lineEdit_command_textChanged(
    const QString &arg1)
{
    ui->lineEdit_args->setText(
        arg1.isEmpty() ? "" : "\"" + g_plugin_input_placeholder + "\"");
}
