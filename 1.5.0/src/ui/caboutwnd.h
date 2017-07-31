#ifndef CABOUTWND_H
#define CABOUTWND_H

#include <QWidget>

class QPushButton;
class CParentDlg;
class QLabel;

namespace Ui {
class CAboutWnd;
}

class CAboutWnd : public QWidget {
    Q_OBJECT

public:
    explicit CAboutWnd(QWidget *parent = 0);

    ~CAboutWnd();

    void UpdateDPI(qreal r);

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:

    void OpenUrlWithChecking(const QUrl &url);

    QPoint m_wnd_move;
    bool m_wnd_pressed;

    QVector<QLabel *> m_clickable_label;

    bool m_movable;

    Ui::CAboutWnd *ui;

public:
signals:
    void SigOpenUrlFailed();

private slots:
    void on_pushButton_close_clicked();

    void SlotBonus();
};

#endif  // CABOUTWND_H
