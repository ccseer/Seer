#ifndef CSETTINGSWND4KEY_H
#define CSETTINGSWND4KEY_H

#include <QKeySequenceEdit>
#include <QMap>
#include <QWidget>

#include "cscreenadapter.h"

namespace Ui {
class CSettingsWnd4Key;
}

class CSettingsWnd4Key : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsWnd4Key(QWidget *parent = 0);
    ~CSettingsWnd4Key();

    void UpdateUI(int title_height, CScreenAdapter *dpi);

    void WriteSettings();

private:
    void InitSC();
    void InitHookKey();

    QMap<QString, QKeySequenceEdit *> m_keys;

    Ui::CSettingsWnd4Key *ui;

signals:
    void SigShowInfoText(const QString &text, ushort time_t);
};

#endif  // CSETTINGSWND4KEY_H
