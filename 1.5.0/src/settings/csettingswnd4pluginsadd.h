#ifndef CSETTINGSWND4PLUGINSADD_H
#define CSETTINGSWND4PLUGINSADD_H

#include <QWidget>
#include "cglobaldata.h"

class QLineEdit;

namespace Ui {
class CSettingsWnd4PluginsAdd;
}

class CSettingsWnd4PluginsAdd : public QWidget {
    Q_OBJECT
public:
    enum FocusControl { Name, Suffix, Command };

    explicit CSettingsWnd4PluginsAdd(QWidget *parent = 0);
    ~CSettingsWnd4PluginsAdd();

    void UpdateUI(CScreenAdapter *dpi);

    //负数表示不是 修改.
    int row_editing;

    void SetData(const QStringList &data);
    void SetFocus(FocusControl wnd);

    Q_SLOT void on_pushButton_cancel_clicked();

protected:
    virtual bool eventFilter(QObject *, QEvent *);

private:
    Ui::CSettingsWnd4PluginsAdd *ui;

signals:
    void SigCancel();
    void SigSubmit(const PluginData &data);

private slots:
    void on_lineEdit_name_textChanged(const QString &arg1);
    void on_lineEdit_suffix_textChanged(const QString &arg1);
    void on_textEdit_args_textChanged();
    void on_pushButton_submit_clicked();

    void on_lineEdit_name_returnPressed();
    void on_lineEdit_suffix_returnPressed();
};

#endif  // CSETTINGSWND4PLUGINSADD_H
