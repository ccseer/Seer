#ifndef CIMGREADER_H
#define CIMGREADER_H

#include <QImage>
#include <QObject>

class CImgReader : public QObject {
    Q_OBJECT
public:
    explicit CImgReader(QObject *parent = 0) : QObject(parent)
    {
    }
    Q_SIGNAL void SigImageSize(const QSize &);
    Q_SIGNAL void SigImageLoaded(const QImage &);
    Q_SIGNAL void SigImageLoadedFull(const QImage &);
    Q_SIGNAL void SigImageLoadedErr(const QString &err);
    Q_SLOT void SlotLoadImage(const QString &fichier);
};

#endif  // CIMGREADER_H
