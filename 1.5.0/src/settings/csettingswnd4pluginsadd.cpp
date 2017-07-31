#include "csettingswnd4pluginsadd.h"
#include "ccomponent.h"
#include "cconfirmdlg.h"
#include "cthememanager.h"
#include "stable.h"
#include "ui_csettingswnd4pluginsadd.h"

#include <qscrollbar.h>
#include <QKeyEvent>
#include <QTextBlock>

CSettingsWnd4PluginsAdd::CSettingsWnd4PluginsAdd(QWidget *parent)
    : QWidget(parent), row_editing(-1), ui(new Ui::CSettingsWnd4PluginsAdd)
{
    ui->setupUi(this);

    ui->lineEdit_name->setPlaceholderText(tr("your plugin name"));

    ui->lineEdit_suffix->setPlaceholderText(
        tr("matched extensions: png jpg bmp"));

    ui->textEdit_args->setPlaceholderText(tr("your plugin.exe [argument] ..."));
    ui->textEdit_args->installEventFilter(this);

    foreach (auto i, this->findChildren<QPushButton *>()) {
        i->setCursor(Qt::PointingHandCursor);
    }

    ui->pushButton_cancel->setText(fa::c(fa::cancel));
    ui->pushButton_submit->setText(fa::c(fa::confirm));

    on_lineEdit_name_textChanged(ui->lineEdit_name->text());
}

CSettingsWnd4PluginsAdd::~CSettingsWnd4PluginsAdd()
{
    delete ui;
}

void CSettingsWnd4PluginsAdd::UpdateUI(CScreenAdapter *dpi)
{
    ui->pushButton_cancel->setFont(fa::font(21 * dpi->ratio()));
    ui->pushButton_submit->setFont(fa::font(21 * dpi->ratio()));

    QFontMetrics fm(qApp->fontMetrics());
    ui->textEdit_args->setFixedHeight(fm.height() * 4);
    ui->lineEdit_name->setFixedWidth(fm.width("your plugin name12345"));

    ui->textEdit_args->setFocusPolicy(Qt::StrongFocus);
    ui->lineEdit_name->setFocusPolicy(Qt::StrongFocus);
    ui->lineEdit_suffix->setFocusPolicy(Qt::StrongFocus);
}

void CSettingsWnd4PluginsAdd::SetData(const QStringList &data)
{
    ui->lineEdit_name->setText(data.first());
    ui->textEdit_args->setPlainText(data.at(1));
    ui->lineEdit_suffix->setText(data.last());
}

void CSettingsWnd4PluginsAdd::SetFocus(FocusControl wnd)
{
    switch (wnd) {
        case CSettingsWnd4PluginsAdd::Name:
            ui->lineEdit_name->setFocus();
            break;
        case CSettingsWnd4PluginsAdd::Suffix:
            ui->lineEdit_suffix->setFocus();
            break;
        case CSettingsWnd4PluginsAdd::Command:
            ui->textEdit_args->setFocus();
            break;
    }
}

void CSettingsWnd4PluginsAdd::on_lineEdit_name_textChanged(const QString &arg1)
{
    ui->pushButton_submit->setEnabled(true);

    const auto name   = arg1.trimmed();
    const auto suffix = ui->lineEdit_suffix->text().trimmed();
    const auto args
        = ui->textEdit_args->document()->firstBlock().text().trimmed();
    if (name.isEmpty() || suffix.isEmpty() || args.isEmpty()) {
        ui->pushButton_submit->setEnabled(false);
    }
}

void CSettingsWnd4PluginsAdd::on_lineEdit_suffix_textChanged(
    const QString & /*arg1*/)
{
    on_lineEdit_name_textChanged(ui->lineEdit_name->text());
}

void CSettingsWnd4PluginsAdd::on_textEdit_args_textChanged()
{
    on_lineEdit_name_textChanged(ui->lineEdit_name->text());
}

void CSettingsWnd4PluginsAdd::on_pushButton_cancel_clicked()
{
    foreach (auto i, this->findChildren<QLineEdit *>()) {
        i->clear();
    }
    emit SigCancel();

    row_editing = -1;
}

void CSettingsWnd4PluginsAdd::on_pushButton_submit_clicked()
{
    PluginData para;
    para.name   = ui->lineEdit_name->text().trimmed();
    para.para   = ui->textEdit_args->document()->firstBlock().text().trimmed();
    para.suffix = QStringList() << ui->lineEdit_suffix->text().trimmed();
    emit SigSubmit(para);
}

bool CSettingsWnd4PluginsAdd::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->textEdit_args
        && (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)) {
        if (QKeyEvent *ev = (QKeyEvent *)e) {
            if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return) {
                return true;
            }
        }
    }
    return false;
}

void CSettingsWnd4PluginsAdd::on_lineEdit_name_returnPressed()
{
    ui->lineEdit_suffix->setFocus();
}

void CSettingsWnd4PluginsAdd::on_lineEdit_suffix_returnPressed()
{
    ui->textEdit_args->setFocus();
    auto cur = ui->textEdit_args->textCursor();
    cur.movePosition(QTextCursor::End);
    ui->textEdit_args->setTextCursor(cur);
}
