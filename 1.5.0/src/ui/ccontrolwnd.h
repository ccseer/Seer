#ifndef CCONTROLWND_H
#define CCONTROLWND_H

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

class CControlWnd : public QWidget {
    Q_OBJECT
public:
    explicit CControlWnd(QWidget* parent = 0);

    void UpdateDPI(qreal r);

    QList<QAction*> Init();

    void UpdateControlsBarData();

    //删除除了 四个成员变量控件以外的 控件
    void ClearControlBar();

    QHBoxLayout* GetBtnsLayout()
    {
        return m_layout;
    }

    int GetCtrlBarHeight();

protected:
    virtual void showEvent(QShowEvent*);

private:
    QIcon GetParentFolderIconFromPath(const QString& p);
    void ExecApp(const QString& app, const QString& args);
    void AddControlsApp();
    QToolButton* CreateBtn(const QString& path);

    QLabel* m_pseparator_const;
    QLabel* m_pseparator_ctrls;
    QToolButton* m_pbtn_default;
    QToolButton* m_pbtn_reveal;
    QHBoxLayout* m_layout;

    QWidget* m_pwnd_bg;

signals:
    void SigHideMainWnd();
    void SigWiggleMainWnd();
    void SigOpenByDefault(const QString& path);
    void SigRevealInExplorer(const QString& path);
};

#endif  // CCONTROLWND_H
