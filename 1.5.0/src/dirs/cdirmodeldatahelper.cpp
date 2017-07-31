#include "cdirmodeldatahelper.h"
#include "calignedmodel.h"
#include "ccomponent.h"
#include "cdiritem.h"
#include "cfolderdatareader.h"
#include "stable.h"
#include "utilities.h"

#include <QFileIconProvider>
#include "qapplication.h"
#include "qdatetime.h"
#include "qdir.h"

CDirModelDataHelper::CDirModelDataHelper(QObject *parent) : QObject(parent)
{
    connect(this, &CDirModelDataHelper::SigNewThread, this,
            &CDirModelDataHelper::SlotNewThread);
}

void CDirModelDataHelper::SetPath(const QString &path)
{
    if (!QFileInfo(path).isDir()) {
        emit SigErr("not dir.");
        return;
    }
    if (m_threads.contains(path)) {
        return;
    }

    CThread *thread           = new CThread;
    CFolderDataReader *reader = new CFolderDataReader;
    reader->moveToThread(thread);

    connect(reader, &CFolderDataReader::destroyed, thread,
            &CThread::deleteLater);
    connect(reader, &CFolderDataReader::SigDataOk, this,
            &CDirModelDataHelper::SlotThreadDone);
    connect(reader, &CFolderDataReader::SigDataErr, this,
            &CDirModelDataHelper::SlotThreadErr);
    connect(this, &CDirModelDataHelper::SigStartReadThread, reader,
            &CFolderDataReader::SlotReadData);

    thread->start();

    emit SigNewThread(path, ReaderPtr(reader));

    emit SigStartReadThread(path);
}

void CDirModelDataHelper::SlotThreadDone(const ModelPtr &ptr,
                                         const QString &dest)
{
    // ui thread.
    m_model = ptr;

    emit SigOk(ptr, dest);
}

void CDirModelDataHelper::SlotThreadErr(const QString &err)
{
    emit SigErr(err);
}

void CDirModelDataHelper::ExpandSubFolder(CDirItem *item)
{
    CDirModelDataHelperSub *m = new CDirModelDataHelperSub;
    m->SetThreadsData(m_threads);
    m_subfolder_items.insert(item->GetPath(), item);
    connect(m, &CDirModelDataHelperSub::SigOk, this,
            &CDirModelDataHelper::SlotSubFolderDone);
    connect(m, &CDirModelDataHelperSub::SigErr, this,
            &CDirModelDataHelper::SlotSubFolderErr);
    connect(m, &CDirModelDataHelperSub::SigNewThread, this,
            &CDirModelDataHelper::SlotNewThread);
    connect(m, &CDirModelDataHelperSub::SigOk, m,
            &CDirModelDataHelperSub::deleteLater);
    connect(m, &CDirModelDataHelperSub::SigErr, m,
            &CDirModelDataHelperSub::deleteLater);

    m->SetPath(item->GetPath());
}

void CDirModelDataHelper::SlotSubFolderDone(const ModelPtr &ptr,
                                            const QString &dest)
{
    CDirItem *item = m_subfolder_items.value(dest);
    if (!item) {
        elprint;
        return;
    }

    //文件夹默认有个子文件 以显示 可展开图标.
    item->takeRow(0);

    auto row = ptr->takeRow(0);
    while (!row.isEmpty()) {
        item->appendRow(row);
        row = ptr->takeRow(0);
    }
    m_subfolder_items.remove(dest);
    emit SigSubOk(item);
}

void CDirModelDataHelper::SlotSubFolderErr(const QString &err)
{
    emit SigSubErr(err);
}

void CDirModelDataHelper::StopThreads()
{
    auto iter = m_threads.begin();
    while (iter != m_threads.end()) {
        if (!iter.value().isNull()) {
            iter.value()->StopThread();
        }
        ++iter;
    }
}

void CDirModelDataHelper::SlotNewThread(const QString &path,
                                        const ReaderPtr &reader)
{
    if (!m_threads.contains(path)) {
        m_threads.insert(path, reader);
    }
}

CDirModelDataHelperSub::CDirModelDataHelperSub(QObject *parent /*= 0*/)
    : CDirModelDataHelper(parent)
{
    disconnect(this, &CDirModelDataHelper::SigNewThread, this,
               &CDirModelDataHelper::SlotNewThread);
}
