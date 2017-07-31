#ifndef CDIRMODELDATAHELPER_H
#define CDIRMODELDATAHELPER_H

#include <QHash>
#include <QObject>
#include <QStandardItemModel>
#include <QString>
#include "qpointer.h"

#include "calignedmodel.h"

class CFolderDataReader;
class CDirItem;

using ReaderPtr = QPointer<CFolderDataReader>;

class CDirModelDataHelper : public QObject {
    Q_OBJECT
public:
    explicit CDirModelDataHelper(QObject* parent = 0);

    void SetPath(const QString& path);

    void ExpandSubFolder(CDirItem* item);

    void StopThreads();

protected:
    QMap<QString, ReaderPtr> m_threads;

    ModelPtr m_model;

    //用于展开子对象时匹配
    QHash<QString, CDirItem*> m_subfolder_items;

protected:
signals:
    // send to parent
    void SigOk(const ModelPtr& ptr, const QString& dest);
    void SigErr(const QString& ptr);

    // send to thread
    void SigStartReadThread(const QString& path);

private:
signals:
    // send to thread
    void SigSubOk(QStandardItem* item);
    void SigSubErr(const QString& err);

    // send to itself
    void SigNewThread(const QString& path, const ReaderPtr& reader);

public slots:
    void SlotNewThread(const QString& path, const ReaderPtr& reader);

protected slots:
    void SlotThreadDone(const ModelPtr& ptr, const QString& dest);
    void SlotThreadErr(const QString& err);

private slots:
    void SlotSubFolderDone(const ModelPtr& ptr, const QString& dest);
    void SlotSubFolderErr(const QString& err);
};

class CDirModelDataHelperSub : public CDirModelDataHelper {
    Q_OBJECT
public:
    CDirModelDataHelperSub(QObject* parent = 0);

    // 用于 SetPath 中匹配
    void SetThreadsData(const QMap<QString, ReaderPtr>& d)
    {
        m_threads = d;
    }

    void ExpandSubFolder(CDirItem* item) = delete;

    void StopThreads() = delete;
};

using DirDataPtr = QSharedPointer<CDirModelDataHelper>;

#endif  // CDIRMODELDATAHELPER_H
