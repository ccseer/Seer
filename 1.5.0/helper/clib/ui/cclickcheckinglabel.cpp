#include "cclickcheckinglabel.h"
#include "stable.h"

const ushort g_double_click_interval = 300;  // doubleClickInterval();

CClickCheckingLabel::CClickCheckingLabel(QWidget *parent) : QLabel(parent)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(g_double_click_interval);
}

void CClickCheckingLabel::mousePressEvent(QMouseEvent *)
{
    // double click
    if (m_timer.isActive()) {
        disconnect(&m_timer, &QTimer::timeout, this,
                   &CClickCheckingLabel::SlotSingleClicked);
        m_timer.stop();
        QTimer::singleShot(0, this, &CClickCheckingLabel::SlotDoubleClicked);
    }
    else {
        connect(&m_timer, &QTimer::timeout, this,
                &CClickCheckingLabel::SlotSingleClicked);
        m_timer.start();
    }
}

void CClickCheckingLabel::mouseMoveEvent(QMouseEvent *)
{
    if (m_timer.isActive()) {
        disconnect(&m_timer, &QTimer::timeout, this,
                   &CClickCheckingLabel::SlotSingleClicked);
        m_timer.stop();
    }
}

void CClickCheckingLabel::SlotSingleClicked()
{
    disconnect(&m_timer, &QTimer::timeout, this,
               &CClickCheckingLabel::SlotSingleClicked);

    emit SigClicked();
}

void CClickCheckingLabel::SlotDoubleClicked()
{
    emit SigDbClicked();
}
