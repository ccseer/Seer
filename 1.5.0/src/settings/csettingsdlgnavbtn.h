#ifndef CSETTINGSDLGNAVBTN_H
#define CSETTINGSDLGNAVBTN_H

#include <QWidget>

namespace Ui {
class CSettingsDlgNavBtn;
}

class CSettingsDlgNavBtn : public QWidget {
    Q_OBJECT

public:
    explicit CSettingsDlgNavBtn(QWidget *parent = 0);
    void Init(const QVariantList &res);
    ~CSettingsDlgNavBtn();

    Q_SIGNAL void Clicked();
    Q_SIGNAL void Move(const QPoint &);

    void SetTitle(bool a);

    void SetChecked(bool checked);
    bool IsChecked()
    {
        return m_checked;
    }

protected:
    QPoint m_wnd_move;
    bool m_wnd_pressed;
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

private:
    bool m_checked;
    bool m_is_title;

    Ui::CSettingsDlgNavBtn *ui;
};

#endif  // CSETTINGSDLGNAVBTN_H
