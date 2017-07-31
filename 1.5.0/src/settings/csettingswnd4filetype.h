#ifndef CSETTINGSWND4FILETYPE_H
#define CSETTINGSWND4FILETYPE_H

#include "cscreenadapter.h"
#include "extensions.h"

#include <QWidget>
class QLabel;

namespace Ui {
class CSettingsWnd4FileType;
}

class CSettingsWnd4FileType : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4FileType(QWidget *parent = 0);
    ~CSettingsWnd4FileType();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *);

private:
    void ReadSettings();
    void EnterDelMode(bool a);
    void SetControlsWndVisible(bool show);

    QStringList m_text_more_new;
    QStringList m_text_more_old;

    QMap<QLabel *, CExtensionList> m_label_extentions;

    Ui::CSettingsWnd4FileType *ui;

signals:
    void SigShowInfoText(const QString &text, ushort time_t);

public slots:
    void WriteSettings();

private slots:
    void on_lineEdit_search_textChanged(const QString &arg1);

    void on_pushButton_add_text_clicked();
    void on_pushButton_remove_text_clicked();
    void on_pushButton_del_clicked();
    void on_pushButton_del_over_clicked();
    void on_comboBox_del_currentIndexChanged(int index);
    void on_comboBox_del_currentTextChanged(const QString &arg1);
    void on_pushButton_markdown_add_clicked();
    void on_pushButton_markdown_del_clicked();
    void on_lineEdit_markdown_css_textChanged(const QString &arg1);
};

#endif  // CSETTINGSWND4FILETYPE_H
