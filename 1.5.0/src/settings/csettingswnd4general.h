#ifndef CSETTINGSWND4GENERAL_H
#define CSETTINGSWND4GENERAL_H

#include <QItemSelection>
#include <QPushButton>
#include <QStandardItemModel>
#include <QWidget>
#include "cscreenadapter.h"
#include "cthememanager.h"
#include "stable.h"

namespace Ui {
class CSettingsWnd4General;
}

class CSettingsWnd4General : public QWidget {
    Q_OBJECT
public:
    explicit CSettingsWnd4General(QWidget *parent = 0);
    ~CSettingsWnd4General();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    void InitLanView();
    const QMap<QssStyle, QPushButton *> GetThemeBtnMap();

    QString m_cur_tr;

    QStandardItemModel m_model;

    Ui::CSettingsWnd4General *ui;

public:
signals:
    void SigRequiredRestart(uint, bool);
    void SigShowInfoText(const QString &text, ushort time_t);

public slots:
    void WriteSettings();

private slots:
    void WriteAutorun();

    void UpdateSettings();

    void on_listView_clicked(const QModelIndex &index);
    void SlotThemeBtnClicked(QAbstractButton *);
};

#endif  // CSETTINGSWND4GENERAL_H
