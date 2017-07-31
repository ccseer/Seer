#ifndef CREGDLG_H
#define CREGDLG_H

#include <QWidget>

namespace Ui {
class CRegDlg;
}

class CRegDlg : public QWidget {
    Q_OBJECT

public:
    explicit CRegDlg(QWidget *parent = 0);
    ~CRegDlg();

    void UpdateDPI(qreal r);

private:
    Ui::CRegDlg *ui;

signals:
    void SigClose();
private slots:
    void on_lineEdit_license_textEdited(const QString &arg1);
    void on_pushButton_buy_clicked();
};

#endif  // CREGDLG_H
