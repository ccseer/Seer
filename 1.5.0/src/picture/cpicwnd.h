#ifndef CPICWND_H
#define CPICWND_H

#include "cabstractwnd.h"
#include "cimageview.h"

#include <QHash>
#include <QLabel>
#include "qtoolbutton.h"

class CPicWnd : public CAbstractWnd {
    Q_OBJECT
public:
    explicit CPicWnd(QWidget *parent = 0);

    virtual QSize GetSize();

    virtual DisplayType Type() const
    {
        return Pic;
    }

protected:
    virtual void InitControlWnd(QHBoxLayout *lay);
    virtual FilePropertyData LoadFileInfoHelper();
    virtual void keyPressEvent(QKeyEvent *);

private:
    void SetActionVisible(QToolButton *act, bool ok);

    QSize m_sz_preview;
    QLabel m_resolution;

    CImageView m_viewer;

    QHash<CImageView::ActionType, QToolButton *> m_pactions;

protected slots:
    virtual void PerformFinished(const QVariant &v);
};

#endif  // CPICWND_H
