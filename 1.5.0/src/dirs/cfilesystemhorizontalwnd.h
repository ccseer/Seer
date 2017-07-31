#ifndef CFILESYSTEMHORIZONTALWND_H
#define CFILESYSTEMHORIZONTALWND_H

#include <QFileInfo>
#include <QItemSelection>
#include <QModelIndex>
#include <QWidget>

#include "cdirmodeldatahelper.h"

class QListView;

namespace Ui {
class CFileSystemHorizontalWnd;
}

class CFileSystemHorizontalWnd : public QWidget {
    Q_OBJECT
public:
    explicit CFileSystemHorizontalWnd(const DirDataPtr &data,
                                      QWidget *parent = 0);
    ~CFileSystemHorizontalWnd();

    QString GetSelectedItemPath();

    void SetModel(const ModelPtr &model);

    void Clear();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    void AppendListView(QStandardItem *item_parent);

    void ItemClicked(QListView *view, const QItemSelection &selected);

    CDirItem *m_pitem_lastdir;

    DirDataPtr m_data;
    ModelPtr m_model;

    Ui::CFileSystemHorizontalWnd *ui;

private:
signals:
    void SigSelectionChanged(const QString &path);

private slots:
    void SlotSubFolderDataReady(QStandardItem *item);
};

#endif  // CFILESYSTEMHORIZONTALWND_H
