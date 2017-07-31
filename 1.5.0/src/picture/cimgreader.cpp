#include "cimgreader.h"
#include "cglobaldata.h"
#include "stable.h"
#include "utilities.h"

#include <QFile>
#include <QFileInfo>
#include <QImageReader>

void CImgReader::SlotLoadImage(const QString &path)
{
    QObject deleter;
    connect(&deleter, &QObject::destroyed, this, &CImgReader::deleteLater);

    bool is_scaled = false;
    // preview
    {
        // normal pic
        QImageReader reader(path);
        reader.setDecideFormatFromContent(true);
        reader.setAutoTransform(true);
        reader.setQuality(100);
        reader.setGamma(1. / 2.2);

        if (!reader.canRead()) {
            emit SigImageLoadedErr(reader.errorString());
            return;
        }

        const auto sz_pix = reader.size();
        QSize sz_res      = sz_pix;
        auto trans        = reader.transformation();
        if (trans.testFlag(QImageIOHandler::TransformationRotate90)
            || trans.testFlag(QImageIOHandler::TransformationMirrorAndRotate90)
            || trans.testFlag(QImageIOHandler::TransformationFlipAndRotate90)
            || trans.testFlag(QImageIOHandler::TransformationRotate270)) {
            sz_res = sz_res.transposed();
        }
        emit SigImageSize(sz_res);

        const auto sz_preview = GetPreviewSize(sz_res);
        if (sz_res != sz_preview) {
            is_scaled = true;
            reader.setScaledSize((sz_res != sz_pix) ? sz_preview.transposed()
                                                    : sz_preview);
        }

        QImage pix_preview;
        try {
            pix_preview = reader.read();
        }
        catch (...) {
            elprint;
            emit SigImageLoadedErr("Exception caught.");
            return;
        }

        if (pix_preview.isNull()) {
            emit SigImageLoadedErr(reader.errorString());
            return;
        }
        emit SigImageLoaded(pix_preview);
    }

    // full
    if (is_scaled) {
        QImage pix_full;
        if (QFileInfo(path).size() < g_img_max_read_sz) {
            // cost time operation
            QImageReader reader(path);
            reader.setDecideFormatFromContent(true);
            reader.setAutoTransform(true);
            pix_full = reader.read();
            if (pix_full.isNull()) {
                qprint << "can't read full image:" << path;
            }
        }
        else {
            qprint << "skip large img:" << path;
        }
        emit SigImageLoadedFull(pix_full);
    }
}
