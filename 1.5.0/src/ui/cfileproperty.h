#ifndef CFILEPROPERTY_H
#define CFILEPROPERTY_H

#include "cplayprocess.h"
#include "csingleton.h"
#include "types.h"

#include <QStandardItemModel>

#define g_pfile_property CSingleton<CFileProperty>::Instance()

class QAction;
class CParentDlg;
class CTableViewWithCopyKey;

class CFileProperty : public QObject {
    Q_OBJECT
public:
    CFileProperty(QObject* parent = NULL);
    ~CFileProperty();

    void Clear();
    void Load(const QString& path);

    Q_SLOT void Hide();
    void Show(const QRect& wnd, qreal ratio);

    QStandardItemModel* GetModel() const
    {
        return m_pmodel;
    }
    void AppendFileProperty(const FilePropertyData& data);

    inline const QString& GetCurPath() const
    {
        return m_path;
    }

protected:
    virtual bool eventFilter(QObject*, QEvent*);

private:
    QList<QStandardItem*> AppenItemsRow(const QString& key, const QString& val);
    QList<QStandardItem*> AppenItemsRow(const QString& key, QStandardItem* val);
    QList<QStandardItem*> AppenItemsRow(const QString& key,
                                        QStandardItem* item_val,
                                        QStandardItem* parent);

    CTableViewWithCopyKey* m_pview;

    QStandardItemModel* m_pmodel;

    QString m_path;

    QAction* m_paction;

    CPlayProcess m_process;

public:
signals:
    void SigViewShow(bool visible);
    void SigPropertyReady(const QString& md5, const QString& target);

public slots:
    void SlotProcessFinished(int code);
    void SlotUpdateShortcut4ViewWnd();
};

#endif  // CFILEPROPERTY_H
