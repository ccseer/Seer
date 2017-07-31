#include "cpicturemanager.h"
#include "cglobaldata.h"
#include "cimgreader.h"
#include "utilities.h"

#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QMovie>

CPictureManager::CPictureManager()
    : CAbstractManager(), m_pmv(new QMovie), m_sz_full(GetMinWndSz(1))
{
}

CPictureManager::~CPictureManager()
{
    delete m_pmv;
}

void CPictureManager::PerformerPrivate()
{
    // mv
    if (g_data->sub_type == S_Movie) {
        m_pmv->setFileName(g_data->path);
        QVariant v;
        if (!m_pmv->isValid()) {
            SetErrMsg();
        }
        else {
            QImageReader reader(g_data->path);
            if (reader.canRead()) {
                m_sz_full = reader.size();
            }
            if (QFileInfo(g_data->path).size() < g_img_max_read_sz) {
                // SeekMovieBackward needs mv to be cached.
                m_pmv->setCacheMode(QMovie::CacheAll);
            }
            v = QVariant::fromValue(m_pmv);
        }
        emit SigPerformFinished(v);
    }
    else {
        // manager 创建时 开启线程. load 完毕后退出线程.
        //	如果 将 thread 设置为非指针,在删除 manager
        //析构时会等待线程执行完成,因为 thread
        //需要阻塞等待正常析构,从而导致界面卡顿
        CThread *loaderThread = new CThread;
        CImgReader *loader    = new CImgReader;
        connect(loader, &CImgReader::destroyed, loaderThread,
                &CThread::deleteLater);
        loader->moveToThread(loaderThread);
        loaderThread->start();

        connect(this, &CPictureManager::SigStartThreadLoad, loader,
                &CImgReader::SlotLoadImage);
        connect(loader, &CImgReader::SigImageSize, this,
                &CPictureManager::SlotImageSize);
        connect(loader, &CImgReader::SigImageLoaded, this,
                &CPictureManager::SlotImageLoaded);
        connect(loader, &CImgReader::SigImageLoadedFull, this,
                &CPictureManager::SlotImageLoadedFull);
        connect(loader, &CImgReader::SigImageLoadedErr, this,
                &CPictureManager::SlotImageLoadFailed);

        emit SigStartThreadLoad(g_data->path);
    }
}

void CPictureManager::SlotImageSize(const QSize &sz)
{
    m_sz_full = sz;
}

void CPictureManager::SlotImageLoaded(const QImage &img)
{
    //主线程执行
    emit SigPerformFinished(img);
}

void CPictureManager::SlotImageLoadedFull(const QImage &img)
{
    emit SigFullImageReady(img);
}

void CPictureManager::SlotImageLoadFailed(const QString &err)
{
    SetErrMsg(err);
    emit SigPerformFinished();
}
