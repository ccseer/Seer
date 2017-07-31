#ifndef CSETTINGSWND4CONTROLSEDIT_H
#define CSETTINGSWND4CONTROLSEDIT_H

#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "cscreenadapter.h"

namespace Ui {
class CSettingsWnd4ControlsEdit;
}

class CSettingsWnd4ControlsEdit : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4ControlsEdit(QCompleter *completer,
                                       QWidget *parent = 0);
    ~CSettingsWnd4ControlsEdit();

    void UpdateUI(CScreenAdapter *dpi);

    void UpdateAddBtn(const QString &path);

    void SetPath(const QString &p);
    QString GetPath() const;

    void SetExtension(const QString &e);
    QString GetExtension() const;

    void SetArgs(const QString &e);
    QString GetArgs() const;

protected:
    virtual bool eventFilter(QObject *, QEvent *);

private:
    Ui::CSettingsWnd4ControlsEdit *ui;

signals:
    void SigEditClicked(CSettingsWnd4ControlsEdit *);
    void SigAddNewContainer();
    void SigRemoveContainer(CSettingsWnd4ControlsEdit *);

public slots:
    void HideEditor(CSettingsWnd4ControlsEdit *);

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_edit_clicked(bool checked);
    void on_pushButton_remove_clicked();
    void on_lineEdit_extension_textChanged(const QString &arg1);
    void on_lineEdit_command_textChanged(const QString &arg1);
};

#endif  // CSETTINGSWND4CONTROLSEDIT_H
