#ifndef QT_H
#define QT_H

#include <qt_windows.h>

#include <QDataStream>
#include <QString>
#include <QtDebug>

#include "seer_ipc.h"

#pragma comment(lib, "User32.lib")

bool invokeWithQt(const QString &file_full_path)
{
    // make sure Seer is running
    if (HWND h = FindWindowEx(nullptr, nullptr, SEER_CLASS_NAME, nullptr)) {
        // 2. prepare data
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << QString(file_full_path);
        COPYDATASTRUCT cd;
        cd.cbData = ba.size();
        cd.lpData = (void *)ba.data();
        cd.dwData = SEER_INVOKE_QT;
        // 3. send
        auto err = SendMessage(h, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cd);
        qDebug() << SUCCEEDED(err) << err;
        return SUCCEEDED(err);
    }
    // Seer is not running
    return false;
}

#endif  // QT_H
