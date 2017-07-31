#ifndef CFOLDERDATAREADER_H
#define CFOLDERDATAREADER_H

#include <QMutex>
#include <QObject>
#include "calignedmodel.h"

class CFolderDataReader : public QObject {
    Q_OBJECT
public:
    explicit CFolderDataReader(QObject* parent = 0)
        : QObject(parent), m_keep_reading(true)
    {
    }
    Q_SIGNAL void SigDataOk(const ModelPtr& ptr, const QString& dest);
    Q_SIGNAL void SigDataErr(const QString& err);
    Q_SLOT void SlotReadData(const QString& dest);

    void StopThread();
    const QString& GetPath() const
    {
        return m_path;
    }

private:
    QString m_path;

    QMutex m_lock;
    bool m_keep_reading;
};

#endif  // CFOLDERDATAREADER_H
