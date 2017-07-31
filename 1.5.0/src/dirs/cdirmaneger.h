#ifndef CDIRMANEGER_H
#define CDIRMANEGER_H

#include <QDateTime>

#include "cabstractmanager.h"
#include "cplayprocess.h"

class CPlayProcess;

struct ComData {
    QString filename;
    uint size;
    uint packed;
    QString ratio;
    QDateTime time;
    bool is_dir;
};
using Databox = QList<ComData>;
Q_DECLARE_METATYPE(Databox)

class CDirManeger : public CAbstractManager {
    Q_OBJECT
public:
    explicit CDirManeger();
    ~CDirManeger();

    virtual void PerformerPrivate();

private:
    void ParseRarLine(const QString &src, Databox &in);
    void ParseZipLine(const QString &src, Databox &in);
    QStringList ParseZipLineData(const QString &src);
    void Sorting(Databox &src);

    QString m_pro_res;

    CPlayProcess m_process;
};

#endif  // CDIRMANEGER_H
