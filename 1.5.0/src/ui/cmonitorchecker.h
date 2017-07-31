#ifndef CMONITORCHECKER_H
#define CMONITORCHECKER_H

#include <QObject>
#include <QScreen>
#include <QTimer>

#include "qt_windows.h"

class CMonitorChecker : public QObject {
    Q_OBJECT
public:
    explicit CMonitorChecker(HWND wnd, QObject *parent = nullptr);
    void Init();

    qreal ratio() const
    {
        return m_ratio;
    }

private:
    HWND m_hwnd_main;

    qreal m_ratio;

    QTimer m_timer;

public:
signals:
    void ratioChanged(qreal ratio);

public slots:
    void Start();
    void Stop();

    bool Check();
};

#endif  // CMONITORCHECKER_H
