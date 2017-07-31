#ifndef CCODEWND_H
#define CCODEWND_H
#include "cabstractwnd.h"

#include <QStackedWidget>
#include "qcombobox.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qtimer.h"

#ifdef HIGHLIGHT
class QsciScintilla;
class QsciLexer;

class CCodeWnd : public CAbstractWnd {
    Q_OBJECT
public:
    static void UpdateFormatLaxer(QsciScintilla *wnd, const FileSubType &t);
    static void InitFormatWnd(QsciScintilla *wnd);
    static QsciLexer *GetLayer(const FileSubType &t);

    explicit CCodeWnd(QWidget *parent = 0);
    ~CCodeWnd();

    virtual DisplayType Type() const
    {
        return Code;
    }
    virtual QSize GetSize();

protected:
    virtual void LoadHelper();

    virtual void InitControlWnd(QHBoxLayout *lay);

    virtual bool eventFilter(QObject *, QEvent *);

private:
    void InitDecodeComboBox(QComboBox *combobox);
    void UpdateNormalLayer();
    void UpdateFormatLaxer(const FileSubType &t);
    void InitNormalWnd();
    void InitFormatWnd();

    void CheckFormatView();

    int GetNormalWndMarginWidth(int line_counter);

    void CloseFileAndTimer();

    QFile m_file;
    QTextStream m_ts;

    QTimer m_timer_read_code;

    QToolButton *m_pbtn_format;
    QComboBox *m_pcombo_decode;

    QsciScintilla *m_pwnd_normal;
    QsciScintilla *m_pwnd_format;
    QStackedWidget *m_pwnd_stack;
};

#endif
#endif  // CCODEWND_H
