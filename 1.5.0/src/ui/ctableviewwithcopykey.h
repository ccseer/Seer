#ifndef CTABLEVIEWWITHCOPYKEY_H
#define CTABLEVIEWWITHCOPYKEY_H

#include <QTreeView>

class CTableViewWithCopyKey : public QTreeView {
    Q_OBJECT
public:
    CTableViewWithCopyKey(QWidget *parent = 0);

    void setModel(QAbstractItemModel *model);

    Q_SLOT void UpdateCurRow();
    Q_SLOT void SaveCurRow();

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *);

    virtual void mouseMoveEvent(QMouseEvent *);

    static QString s_selected_view_key;
};

#endif  // CTABLEVIEWWITHCOPYKEY_H
