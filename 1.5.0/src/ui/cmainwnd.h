#ifndef CMAINWND_H
#define CMAINWND_H

#include "cmonitorchecker.h"
#include "cparentdlg.h"
#include "cpluginshelper.h"
#include "stable.h"
#include "cfileproperty.h"
#include "types.h"

#include <QFileInfo>
#include <QTimer>

class QStackedWidget;
class QLabel;
class QScreen;
class QToolButton;

class CUnknownWnd;
class CAbstractWnd;
class CControlWnd;

class CMainWnd : public QDialog {
    Q_OBJECT
public:
    explicit CMainWnd(CMonitorChecker *dpi, QWidget *parent = 0);
    ~CMainWnd();

    bool RespondDClick(const QPoint &pt);

    inline bool IsSameSourceFile(const QString &p)
    {
        return g_pfile_property->GetCurPath() == p;
    }

    // update shortcuts when settings changed
    Q_SIGNAL void UpdateShortcuts();

    void CleanWnd();
    void ShowWnd();

    void UpdateDPI();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

    virtual void keyPressEvent(QKeyEvent *e);

private:
    // title 和  bottom 的显示效果
    QGraphicsEffect *GetShadowEff4FloatingWnd(qreal y_offset, qreal br);
    CAbstractWnd *GetContentWnd(DisplayType t);
    void InitConstantWnd();
    void InitTitleBar();
    void InitShortcuts();
    void InitControlWnd();
    void DetectFileType(const QFileInfo &info);
    bool IsAvailableContentView() const;

    // content
    QStackedWidget *m_pwnd_stack;
    QLabel *m_pwnd_load;
    CAbstractWnd *m_pwnd_unknow;
    CAbstractWnd *m_pwnd_cur;

    // title
    QToolButton *m_pbtn_info;
    QLabel *m_pspacer_infobtn;

    CControlWnd *m_pwnd_control;

    CPluginsHelper m_plugins;

    QTimer m_mouse_tracker;

    CMonitorChecker *m_dpi;

    Ui::CParentDlg *ui;

signals:
    void SigHideWnd();
    void SigWaggleWnd();
    void SigMaximizeWnd();
    void SigTitleMouseEvt(bool is_press);
    void SigAfterLoaded(const QSize &);
    void SigBe4Load();

public slots:
    void Load(const QString &path);
    void LoadLoading();
    void LoadUnknownFile();

    void HideWnd();

    void OpenByDefault(const QString &path);
    void RevealInExplorer(const QString &path);
    void ShowToast(const QString &str);

    void CheckMousePos();

private slots:
    void SlotLoaded();
    void DelContentWnd();
};

#endif  // CMAINWND_H
