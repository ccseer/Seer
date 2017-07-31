#ifndef CABSTRACTMANAGER_H
#define CABSTRACTMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QVariant>

#include "stable.h"

class CAbstractManager : public QObject {
    Q_OBJECT
public:
    explicit CAbstractManager(QObject* parent = NULL) : QObject(parent)
    {
    }
    ~CAbstractManager()
    {
    }

    virtual void PerformerPrivate()
    {
    }

protected:
    const QString& GetErrMsg() const
    {
        return m_err;
    }
    void SetErrMsg(const QString& err = tr("Unknown"))
    {
        m_err = err;
    }

protected:
    QString m_err;
    friend class CAbstractWnd;

public:
signals:
    void SigPerformFinished(const QVariant& v = QVariant{});
};
using ManagerPtr = QSharedPointer<CAbstractManager>;

#endif  // CABSTRACTMANAGER_H
