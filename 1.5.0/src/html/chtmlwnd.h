#ifndef CTEXTWND_H
#define CTEXTWND_H

#include <QStackedWidget>
#include "cabstractwnd.h"
#ifdef HIGHLIGHT
#include <qscilexerhtml.h>
#include <qsciscintilla.h>
#endif
#include <QWebEngineSettings>
#include <QWebEngineView>
#include "cwebpage.h"

class CHtmlWnd : public CAbstractWnd {
    Q_OBJECT
public:
    explicit CHtmlWnd(QWidget *parent = 0);
    ~CHtmlWnd();

    virtual void InitControlWnd(QHBoxLayout *lay);

    virtual QSize GetSize();

    virtual DisplayType Type() const
    {
        return Html;
    }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    void InitWeb();
    void InitHtmlCodeWnd();
    QString ReadFromTextStream(int line_t = 200);
    void LoadHtmlCode();

    QStackedWidget *m_pwnd_container;
    QWebEngineView *m_pwnd_web;
    CWebPage *m_ppage;
    bool m_is_first;

#ifdef HIGHLIGHT
    QsciScintilla *m_pwnd_html;
#endif
    QToolButton *m_pbtn_html;

protected slots:
    virtual void PerformFinished(const QVariant &v);

private slots:
    void SlotWebLoaded(bool ok);
};

#endif  // CTEXTWND_H
