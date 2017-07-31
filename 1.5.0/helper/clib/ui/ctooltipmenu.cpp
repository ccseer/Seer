#include "ctooltipmenu.h"

#include <qevent.h>
#include <QToolTip>

CTooltipMenu::CTooltipMenu(QWidget *parent) : QMenu(parent)
{
}

bool CTooltipMenu::event(QEvent *e)
{
    const QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    if (helpEvent->type() == QEvent::ToolTip && activeAction() != 0) {
        QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
    }
    else {
        QToolTip::hideText();
    }
    return QMenu::event(e);
}
