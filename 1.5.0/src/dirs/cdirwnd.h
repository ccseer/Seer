#ifndef CDIRWND_H
#define CDIRWND_H

#include "cabstractwnd.h"
#include "cdirmaneger.h"
#include "cdirmodeldatahelper.h"
#include "cfilesystemhorizontalwnd.h"

#include "qfilesystemmodel.h"
#include "qstackedwidget.h"

class CFileSystemModel;
class CAlignedModel;
class CDirTreeView;
class QTreeView;

class CDirWnd : public CAbstractWnd {
    Q_OBJECT

public:
    explicit CDirWnd(QWidget* parent = 0);
    ~CDirWnd();

    virtual QSize GetSize();
    virtual DisplayType Type() const
    {
        return Dir;
    }

protected:
    virtual void LoadHelper();

    virtual void InitControlWnd(QHBoxLayout* lay);

private:
    QString GetCurIndexPath4Folder();
    void InitCompression();
    void InitFolder();
    void InitTreeView(QTreeView* view);

    void SetBtnsEnable4Folder(bool arg);
    void UpdateOpenByDefApp(const QString& p);

    void Copy();

    QToolButton* m_pbtn_folder_tree;
    QToolButton* m_pbtn_folder_column;

    QStackedWidget* m_pwnd_stack;

    // folder
    QToolButton* m_pbtn_open;
    QToolButton* m_pbtn_rev;
    QToolButton* m_pbtn_eye;

    CDirTreeView* m_pview_folder_tree;
    CFileSystemHorizontalWnd* m_pview_folder_column;
    DirDataPtr m_folder_data;

    // compression
    QTreeView* m_pview_com_tree;
    CFileSystemHorizontalWnd* m_pview_com_column;
    DirDataPtr m_com_data;

protected slots:
    virtual void PerformFinished(const QVariant& v);
};
#endif  // CDIRWND_H
