#ifndef CABSTRACTWND_H
#define CABSTRACTWND_H

#include "cabstractmanager.h"
#include "cglobaldata.h"
#include "cmonitorchecker.h"

#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

class QLabel;
class QVBoxLayout;

class CAbstractWnd : public QWidget {
    Q_OBJECT
public:
    explicit CAbstractWnd(const ManagerPtr& manager, QWidget* parent = 0);
    ~CAbstractWnd();

    void Init(CMonitorChecker* dpi);

    void Load();

    virtual DisplayType Type() const = 0;

    //返回实际内容大小
    virtual QSize GetSize();

    void LoadFileInfo();

    virtual void InitControlWnd(QHBoxLayout* lay);

protected:
    virtual void UpdateDPI(qreal){}

    virtual FilePropertyData LoadFileInfoHelper()
    {
        return {};
    }

    void SetWnd(QWidget* wnd);

    void SetErrMsg(const QString& err);
    QString GetErrMsg() const;

    QToolButton* GetBottomBtn(const QPixmap& normal,
                              const QPixmap& active,
                              const QString& tooltip,
                              const QString& key = "",
                              QAction* act       = NULL);

    QToolButton* GetBottomBtn(const fa::icon & text,
                              const QString& tooltip,
                              const QString& key = "",
                              QAction* act       = NULL);

    ManagerPtr m_manager;

    CMonitorChecker* m_dpi;

    QElapsedTimer m_t_debug;

private:
    QVBoxLayout* m_playout;


public:
signals:
    void SigLoaded();
    void SigErr(const QString& err = "");
    void SigOpenByDefault(const QString& file);
    void SigRevealInExplorer(const QString& file);
    void SigLoadNewPath(const QString& path);
    void SigHideMainWnd();
    void SigWiggleMainWnd();

    void SigShowMsg(const QString& str);

protected slots:
    virtual void LoadHelper();
    virtual void PerformFinished(const QVariant& /*v*/)
    {
    }
};

#endif  // CABSTRACTWND_H
