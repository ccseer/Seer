#ifndef CPICTUREMANAGER_H
#define CPICTUREMANAGER_H

#include <qbuffer.h>
#include <QPixmap>

#include "cabstractmanager.h"
#include "ccomponent.h"

class CPictureManager : public CAbstractManager {
    Q_OBJECT
public:
    explicit CPictureManager();
    ~CPictureManager();

    virtual void PerformerPrivate();

    const QSize& GetImgFullSz() const
    {
        return m_sz_full;
    }

private:
    QMovie* m_pmv;

    QSize m_sz_full;

private:
signals:
    void SigFullImageReady(const QImage& pix);

    void SigStartThreadLoad(const QString&);

private slots:
    void SlotImageSize(const QSize& sz);
    void SlotImageLoaded(const QImage& img);
    void SlotImageLoadedFull(const QImage& img);
    void SlotImageLoadFailed(const QString& err);
};

#endif  // CPICTUREMANAGER_H
