#ifndef CSCREENADAPTER_H
#define CSCREENADAPTER_H

#include <QObject>
#include <QWidget>
#include <QWindow>

// 需要以 QWindow 为单位 -> screenChanged -> ratioChanged -> updateUI
class CScreenAdapter : public QObject {
    Q_OBJECT

    Q_PROPERTY(qreal ratio READ ratio NOTIFY ratioChanged)
public:
    CScreenAdapter();

    // 需要在显示之后调用
    Q_INVOKABLE void init(QWidget *wnd);
    Q_INVOKABLE void init(QWindow *wnd);

    qreal ratio() const
    {
        return m_ratio;
    }

    qreal x();
    qreal x3()
    {
        return x() * 3;
    }
    qreal x5()
    {
        return x() * 5;
    }
    qreal x8()
    {
        return x() * 8;
    }
    qreal getSize(qreal sz);

    static QWidget *getTopWnd(QWidget *w);
    static qreal getRatio(QWidget *w);
    static qreal getRatio(QScreen *w);

    static QString UpdateQSS(const QString &qss_path, qreal r);
    QString UpdateQSS(const QString &qss_path);

private:
    CScreenAdapter(QWindow *wnd);
    void DoConnect(QWindow *wnd);

    qreal m_ratio;

    QWindow *m_associated_wnd;

signals:
    void ratioChanged(qreal ratio);

private slots:
    void ScreenChanged(QScreen *screen);
};

#endif  // CSCREENADAPTER_H
