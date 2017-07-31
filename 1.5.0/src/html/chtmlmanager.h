#ifndef CTEXTMANAGER_H
#define CTEXTMANAGER_H

#include "cabstractmanager.h"
#include "cglobaldata.h"
#include "cplayprocess.h"

class CPlayProcess;

class CHtmlManager : public CAbstractManager {
    Q_OBJECT
public:
    explicit CHtmlManager();
    ~CHtmlManager();

    QString GetConstantHtml4Markdown(const QString& input);

    virtual void PerformerPrivate();

protected:
    QString WriteMD2File(const QString& res);
    void LoadMarkdown();
    void LoadWeb();

    void ExecProcess(const QString& exe_path,
                     const QString& path,
                     const QStringList& args = {});

private:
    QByteArray m_pro_res;
    CPlayProcess m_process;
};

#endif  // CTEXTMANAGER_H
