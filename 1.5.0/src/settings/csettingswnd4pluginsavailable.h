#ifndef CSETTINGSWND4PLUGINSAVAILABLE_H
#define CSETTINGSWND4PLUGINSAVAILABLE_H

#include <QListView>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTextBrowser>
#include <QWidget>
#include "cscreenadapter.h"

namespace Ui {
class CSettingsWnd4PluginsAvailable;
}

class CSettingsWnd4PluginsAvailable : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4PluginsAvailable(QWidget *parent = 0);
    ~CSettingsWnd4PluginsAvailable();

    void UpdateUI(CScreenAdapter *dpi);

private slots:
    void on_pushButton_dl_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    void InitData();

    const QString m_const_text;

    QStandardItemModel m_model;
    QListView *m_pview;

    QSplitter *m_psplitter;
    QTextBrowser *m_peditor;

    Ui::CSettingsWnd4PluginsAvailable *ui;
};

#endif  // CSETTINGSWND4PLUGINSAVAILABLE_H
