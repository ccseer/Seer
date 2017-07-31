#include "cregdlg.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "cparentdlg.h"
#include "global_headers.h"
#include "settingshelper.h"
#include "ui_cregdlg.h"
#include "utilities.h"

#include <QTimer>
#include <qclipboard>

CRegDlg::CRegDlg(QWidget *parent) : QWidget(parent), ui(new Ui::CRegDlg)
{
    ui->setupUi(this);

    ui->lineEdit_id->setReadOnly(true);
    ui->lineEdit_id->setText(CLicenseManager::GetCurID());
    ui->pushButton_buy->setToolTip(web_order);
    ui->pushButton_buy->setCursor(Qt::PointingHandCursor);

    if (CLicenseManager::CheckFromLocal()) {
        ui->pushButton_cancel->setVisible(false);
        ui->pushButton_buy->setVisible(false);
        ui->lineEdit_license->setReadOnly(true);
        ui->lineEdit_license->setText(CLicenseManager::GetCurLicense());
        ui->pushButton_licensed->setText(tr("OK"));
    }
    else {
        ui->pushButton_licensed->setVisible(false);
        ui->lineEdit_license->setClearButtonEnabled(true);
    }
    connect(ui->pushButton_cancel, &QPushButton::clicked, this,
            &CRegDlg::SigClose);
    connect(ui->pushButton_licensed, &QPushButton::clicked, this,
            &CRegDlg::SigClose);

    connect(ui->pushButton_copy, &QPushButton::clicked, [=]() {
        ui->pushButton_copy->setText(tr("Copied"));
        qApp->clipboard()->setText(ui->lineEdit_id->text());
        QTimer::singleShot(5000, ui->pushButton_copy,
                           [=]() { ui->pushButton_copy->setText(tr("Copy")); });
    });
    UpdateDPI(1);
}

CRegDlg::~CRegDlg()
{
    delete ui;
}

void CRegDlg::UpdateDPI(qreal /*r*/)
{
    auto i         = ui->label_id;
    const auto max = qMax(i->fontMetrics().width(i->text()),
                          i->fontMetrics().width(ui->label_license->text()));
    i->setFixedWidth(max);
    ui->label_license->setFixedWidth(max);
}

void CRegDlg::on_pushButton_buy_clicked()
{
    if (!OpenByDefaultApp(
            QUrl(ui->pushButton_buy->toolTip(), QUrl::TolerantMode))) {
        if (auto p = GetParentDlgPtr) {
            C::Ui::Ani::WaggleWnd(p);
        }
    }
}

void CRegDlg::on_lineEdit_license_textEdited(const QString &arg1)
{
    const auto key = arg1.trimmed();
    if (key.length() != Encrypt("").length()) {
        ui->pushButton_cancel->setText(tr("Cancel"));
        return;
    }

    if (CLicenseManager::IsLicenseMatchAllIDs(key)) {
        ui->pushButton_cancel->setVisible(false);
        ui->pushButton_buy->setVisible(false);
        ui->pushButton_licensed->setVisible(true);
        ui->pushButton_licensed->setVisible(true);
        ui->lineEdit_license->setClearButtonEnabled(false);
        ui->lineEdit_license->setReadOnly(true);
    }
    else {
        ui->pushButton_cancel->setText(tr("Invalid License"));
    }
}
