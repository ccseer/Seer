#ifndef CDIRTREEVIEW_H
#define CDIRTREEVIEW_H

#include <QTreeView>
#include "calignedmodel.h"
#include "cdirmodeldatahelper.h"

class CDirTreeView : public QTreeView {
public:
    Q_OBJECT
public:
    CDirTreeView(const DirDataPtr &data, QWidget *parent = 0);

    void SetModel(const ModelPtr &model);

    QString GetSelectedItemPath();

protected:
    virtual void mousePressEvent(QMouseEvent *event);

private:
    void TreeCtrlClicked(const QModelIndex &index);

    bool m_is_first_load;

    CDirItem *m_pitem_lastclick;

    ModelPtr m_model;

    DirDataPtr m_data;

private:
signals:
    void SigSelectionChanged(const QString &path);

private slots:
    void SlotSubFolderDataReady(QStandardItem *item);
};

#endif  // CDIRTREEVIEW_H
