#ifndef CTOOLTIPMENU_H
#define CTOOLTIPMENU_H

#include <QMenu>

class CTooltipMenu : public QMenu {
    Q_OBJECT
public:
    CTooltipMenu(QWidget *parent = 0);

    virtual bool event(QEvent *e);
};

#endif  // CTOOLTIPMENU_H
