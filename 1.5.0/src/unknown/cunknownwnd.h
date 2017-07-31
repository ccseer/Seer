#ifndef CUNKNOWNWND_H
#define CUNKNOWNWND_H

#include "cabstractwnd.h"

class CTableViewWithCopyKey;

class CUnknownWnd : public CAbstractWnd {
    Q_OBJECT
public:
    explicit CUnknownWnd(QWidget *parent = 0);

    virtual DisplayType Type() const
    {
        return None;
    }

protected:
    virtual void LoadHelper();

    virtual QSize GetSize();

private:
    CTableViewWithCopyKey *m_pview;
};

#endif  // CUNKNOWNWND_H
