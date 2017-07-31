#ifndef CSETTINGSWND4PLUGINS_H
#define CSETTINGSWND4PLUGINS_H

#include <QWidget>

#include "csettingswnd4pluginsadd.h"
#include "csettingswnd4pluginsavailable.h"

class QStandardItemModel;

namespace Ui {
class CSettingsWnd4Plugins;
}

class CSettingsWnd4Plugins : public QWidget {
    Q_OBJECT
public:
    explicit CSettingsWnd4Plugins(QWidget *parent = 0);
    ~CSettingsWnd4Plugins();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *);

private:
    void InitData();
    void SetAddPluginVisibel(bool arg);

    QStandardItemModel *m_pmodel;

    CSettingsWnd4PluginsAdd m_wnd_add;

    CSettingsWnd4PluginsAvailable m_wnd_available;

    Ui::CSettingsWnd4Plugins *ui;

signals:
    void SigShowInfoText(const QString &text, ushort time_t);

public slots:
    void WriteSettings();

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_edit_clicked();
    void on_pushButton_del_clicked();

    void SlotAddPluginSubmit(const PluginData &data);
    void on_lineEdit_search_textChanged(const QString &arg1);
    void on_pushButton_about_plugin_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
};

#endif  // CSETTINGSWND4PLUGINS_H
