#ifndef CCLIPBOARDHELPER_H
#define CCLIPBOARDHELPER_H

#include <QObject>
#include <QString>

using FmtBytes = QPair<QString, QByteArray>;

class CClipboardHelper : public QObject {
    Q_OBJECT
public:
    explicit CClipboardHelper(QObject *parent = 0);

    void Backup();
    void Restore();

private:
    QList<FmtBytes> m_old_clipboard_data;

signals:

public slots:
};

#endif  // CCLIPBOARDHELPER_H
