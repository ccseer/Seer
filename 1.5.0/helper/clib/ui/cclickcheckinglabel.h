#ifndef CCLICKCHECKINGLABEL_H
#define CCLICKCHECKINGLABEL_H

#include <QLabel>
#include <QTimer>

class CClickCheckingLabel : public QLabel {
    Q_OBJECT

public:
    CClickCheckingLabel(QWidget *parent = 0);

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

private:
    // label_head double click
    QTimer m_timer;

public:
signals:
    void SigClicked();
    void SigDbClicked();

private slots:
    void SlotSingleClicked();
    void SlotDoubleClicked();
};

#endif  // CCLICKCHECKINGLABEL_H
