#include "ccodewnd.h"
#include "ccomboboxdelegate4separator.h"
#include "ccomponent.h"
#include "cthememanager.h"
#include "settingshelper.h"

#include <QAction>
#include <QDomDocument>
#include <QFontMetrics>
#include <QScrollBar>
#include "qapplication.h"
#include "qcombobox.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qfileinfo.h"
#include "qjsondocument.h"
#include "qlistview.h"
#include "qtextcodec.h"

#ifdef HIGHLIGHT
#include "qsciLexercpp.h"
#include "qscilexer.h"
#include "qscilexerbash.h"
#include "qscilexerbatch.h"
#include "qscilexercmake.h"
#include "qscilexercsharp.h"
#include "qscilexercss.h"
#include "qscilexerd.h"
#include "qscilexerdiff.h"
#include "qscilexerfortran.h"
#include "qscilexerfortran77.h"
#include "qscilexerhtml.h"
#include "qscilexeridl.h"
#include "qscilexerjava.h"
#include "qscilexerjavascript.h"
#include "qscilexerlua.h"
#include "qscilexermakefile.h"
#include "qscilexermatlab.h"
#include "qscilexeroctave.h"
#include "qscilexerpascal.h"
#include "qscilexerperl.h"
#include "qscilexerpostscript.h"
#include "qscilexerpov.h"
#include "qscilexerproperties.h"
#include "qscilexerpython.h"
#include "qscilexerruby.h"
#include "qscilexerspice.h"
#include "qscilexersql.h"
#include "qscilexertcl.h"
#include "qscilexertex.h"
#include "qscilexervhdl.h"
#include "qscilexerxml.h"
#include "qscilexeryaml.h"
#include "qsciscintilla.h"
// self-made
#include "qscilexerplain.h"

CCodeWnd::CCodeWnd(QWidget *parent /*= 0*/)
    : CAbstractWnd(ManagerPtr(new CAbstractManager), parent),
      m_pwnd_normal(new QsciScintilla),
      m_pwnd_format(new QsciScintilla),
      m_pcombo_decode(new QComboBox),
      m_pwnd_stack(new QStackedWidget)
{
    InitNormalWnd();
    InitFormatWnd();
    m_pwnd_stack->addWidget(m_pwnd_normal);
    m_pwnd_stack->addWidget(m_pwnd_format);
    SetWnd(m_pwnd_stack);
    m_pwnd_normal->viewport()->installEventFilter(this);
    m_pwnd_format->viewport()->installEventFilter(this);

    m_timer_read_code.setInterval(200);
    connect(&m_timer_read_code, &QTimer::timeout, [=]() {
        if (!m_ts.atEnd()) {
            m_pwnd_normal->append(m_ts.read(g_code_max_t).toUtf8());
        }
        else {
            CloseFileAndTimer();
        }
    });
    connect(&m_file, &QFile::aboutToClose, [=]() {
        m_pbtn_format->defaultAction()->setEnabled(true);
        m_pcombo_decode->setEnabled(true);
        m_pwnd_normal->setCursorPosition(0, 0);
    });
}

CCodeWnd::~CCodeWnd()
{
    CloseFileAndTimer();
    if (auto l = m_pwnd_normal->lexer()) {
        delete l;
    }
    if (auto l = m_pwnd_format->lexer()) {
        delete l;
    }
}

void CCodeWnd::InitDecodeComboBox(QComboBox *combobox)
{
    auto list = new QListView(combobox);
    list->verticalScrollBar()->setObjectName("sub");
    combobox->setView(list);
    combobox->setMaxVisibleItems(4);
    {
        combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        combobox->addItem("System");
        combobox->addItem("UTF-8");
        list->setItemDelegate(new CComboBoxDelegate4Separator(combobox));
        auto codes = QTextCodec::availableCodecs();
        codes.removeAll("System");
        codes.removeAll("UTF-8");
        std::stable_sort(codes.begin(), codes.end(),
                         [](const QByteArray &a, const QByteArray &b) {
                             return QString(a).toLower() < QString(b).toLower();
                         });
        foreach (const auto &i, codes) {
            combobox->addItem(QString{i});
        }
        combobox->insertSeparator(2);
    }
}

void CCodeWnd::InitNormalWnd()
{
    m_pwnd_normal->setUtf8(true);
    m_pwnd_normal->setContextMenuPolicy(Qt::NoContextMenu);
    m_pwnd_normal->setReadOnly(true);
    m_pwnd_normal->setFrameStyle(QFrame::NoFrame);
    m_pwnd_normal->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pwnd_normal->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // fold
    m_pwnd_normal->setFolding(QsciScintilla::NoFoldStyle);
    m_pwnd_normal->setBraceMatching(QsciScintilla::NoBraceMatch);
    m_pwnd_normal->setCaretLineVisible(false);
}

void CCodeWnd::InitFormatWnd()
{
    InitFormatWnd(m_pwnd_format);
}

void CCodeWnd::InitControlWnd(QHBoxLayout *lay)
{
    m_pbtn_format = GetBottomBtn(fa::ctrl_code, tr("Format Code"));
    m_pbtn_format->defaultAction()->setCheckable(true);
    lay->addWidget(m_pbtn_format);
    lay->addStretch();
    lay->addWidget(m_pcombo_decode);
    InitDecodeComboBox(m_pcombo_decode);

    CheckFormatView();

    connect(m_pwnd_stack, &QStackedWidget::currentChanged, [=]() {
        m_pcombo_decode->setVisible(m_pwnd_stack->currentWidget()
                                    == m_pwnd_normal);
    });
    connect(
        m_pcombo_decode,
        (void (QComboBox::*)(const QString &)) & QComboBox::currentIndexChanged,
        [=](const QString &str) {
            if (!m_pcombo_decode->isEnabled()) {
                return;
            }

            qApp->setOverrideCursor(Qt::WaitCursor);
            {
                m_file.setFileName(g_data->path);
                if (m_file.open(QFile::ReadOnly | QFile::Text)) {
                    m_pcombo_decode->setEnabled(false);
                    // decode
                    m_ts.setDevice(&m_file);
                    m_ts.setAutoDetectUnicode(true);
                    m_ts.setCodec(str.toLatin1());

                    // read
                    m_pwnd_normal->setText(m_ts.read(g_code_max_t).toUtf8());

                    m_ts.atEnd() ? m_file.close() : m_timer_read_code.start();
                }
                else {
                    m_file.exists() ? emit SigErr() : emit SigHideMainWnd();
                }
            }
            qApp->restoreOverrideCursor();
        });

    connect(
        m_pbtn_format->defaultAction(), &QAction::triggered, this,
        [=](bool checked) {
            if (checked) {
                m_pwnd_format->setWrapMode(
                    g_pini->GetIni(Ini::text_wrap_b).toBool()
                        ? QsciScintilla::WrapWord
                        : QsciScintilla::WrapNone);
                m_pwnd_format->setWrapVisualFlags(QsciScintilla::WrapFlagNone);

                auto lam = [=](const QString &err, int line, int col) {
                    m_pwnd_format->setText(
                        tr("Format failed.") + "\n\n" + tr("Reason:") + err
                        + "\n" + tr("Line: ") + QString::number(line) + "\t"
                        + tr("Column: ") + QString::number(col));
                    m_pwnd_format->setCursorPosition(4, 0);
                    m_pwnd_normal->setFocus();
                    m_pwnd_normal->setCursorPosition(line, col);
                    m_pwnd_normal->setFirstVisibleLine(line);
                };

                qApp->setOverrideCursor(Qt::WaitCursor);
                if (g_data->suffix == "json") {
                    UpdateFormatLaxer(S_JavaScript);
                    QJsonParseError error;
                    auto doc = QJsonDocument::fromJson(
                        m_pwnd_normal->text().toUtf8(), &error);
                    if (doc.isNull()) {
                        int line  = 0;
                        int index = 0;
                        m_pwnd_normal->lineIndexFromPosition(error.offset,
                                                             &line, &index);
                        lam(error.errorString(), line, index);
                    }
                    else {
                        m_pwnd_format->setText(
                            doc.toJson(QJsonDocument::Indented));
                    }
                }
                else {
                    UpdateFormatLaxer(S_Xml);
                    QDomDocument input;
                    QString err;
                    int row = 0;
                    int col = 0;
                    if (input.setContent(m_pwnd_normal->text().toUtf8(), &err,
                                         &row, &col)) {
                        QDomDocument output(input);
                        QByteArray res;
                        QTextStream stream(&res);
                        output.save(stream, 4);
                        m_pwnd_format->setText(res);
                    }
                    else {
                        lam(err, row, col);
                    }
                }

                qApp->restoreOverrideCursor();
                m_pwnd_stack->setCurrentWidget(m_pwnd_format);
            }
            else {
                m_pwnd_stack->setCurrentWidget(m_pwnd_normal);
            }
        });
}

void CCodeWnd::CheckFormatView()
{
    const auto suf = g_data->suffix;
    if (suf == "json" || suf == "xml" || suf == "xaml") {
        m_pbtn_format->defaultAction()->setChecked(false);
        m_pbtn_format->defaultAction()->setEnabled(false);
        m_pbtn_format->setVisible(true);
    }
    else {
        m_pbtn_format->setVisible(false);
    }
}

void CCodeWnd::CloseFileAndTimer()
{
    if (m_timer_read_code.isActive()) {
        m_timer_read_code.stop();
    }
    if (m_file.isOpen()) {
        m_file.close();
    }
}

// this is a synchronous loading.
void CCodeWnd::LoadHelper()
{
    m_pwnd_stack->setCurrentWidget(m_pwnd_normal);
    m_pcombo_decode->setEnabled(false);

    m_file.setFileName(g_data->path);
    if (!m_file.open(QFile::ReadOnly | QFile::Text)) {
        SetErrMsg(m_file.errorString());
        emit SigErr();
        return;
    }

    UpdateNormalLayer();

    // settings
    {
        m_pwnd_normal->setWrapMode(g_pini->GetIni(Ini::text_wrap_b).toBool()
                                       ? QsciScintilla::WrapWord
                                       : QsciScintilla::WrapNone);
        m_pwnd_normal->setWrapVisualFlags(QsciScintilla::WrapFlagNone);

        // code 显示行号. 计算行号的时间消耗非常低,可实现计算出行数再算出数字宽.
        if (g_pini->GetIni(Ini::text_line_num_b).toBool()) {
            uint line_counter = 0;
            while (!m_file.atEnd()) {
                m_file.readLine();
                ++line_counter;
            }
            m_file.seek(0);

            m_pwnd_normal->setMarginWidth(
                1, GetNormalWndMarginWidth(line_counter));
            m_pwnd_normal->setMarginLineNumbers(1, true);
        }
        else {
            m_pwnd_normal->setMarginWidth(1, qApp->fontMetrics().xHeight() * 6);
            m_pwnd_normal->setMarginLineNumbers(1, false);
        }
    }

    // decode
    {
        m_ts.setDevice(&m_file);
        m_ts.setAutoDetectUnicode(true);
        const QByteArray charset
            = C::Core::Funcs::GetCharSetName(m_file.readAll());
        m_file.seek(0);
        QString decode = "UTF-8";
        if (!charset.isEmpty()) {
            m_ts.setCodec(QTextCodec::codecForName(charset));
            decode = charset;
        }
        int index = m_pcombo_decode->findText(decode, Qt::MatchFixedString);
        m_pcombo_decode->setCurrentIndex(
            index == -1
                ? m_pcombo_decode->findText("UTF-8", Qt::MatchFixedString)
                : index);
    }
    // read
    m_pwnd_normal->setText(m_ts.read(g_code_max_t).toUtf8());

    m_ts.atEnd() ? m_file.close() : m_timer_read_code.start();

    emit SigLoaded();
}

QsciLexer *CCodeWnd::GetLayer(const FileSubType &t)
{
    QsciLexer *lex = nullptr;
    switch (t) {
        case S_Bash:
            lex = new QsciLexerBash;
            break;
        case S_Batch:
            lex = new QsciLexerBatch;
            break;
        case S_Cmake:
            lex = new QsciLexerCMake;
            break;
        case S_Cpp:
            lex = new QsciLexerCPP;
            break;
        case S_C_Shap:
            lex = new QsciLexerCSharp;
            break;
        case S_Css:
            lex = new QsciLexerCSS;
            break;
        case S_Diff:
            lex = new QsciLexerDiff;
            break;
        case S_Fortran77:
            lex = new QsciLexerFortran77;
            break;
        case S_Fortran90:
            lex = new QsciLexerFortran;
            break;
        case S_Html:
            lex = new QsciLexerHTML;
            break;
        case S_Java:
            lex = new QsciLexerJava;
            break;
        case S_Lua:
            lex = new QsciLexerLua;
            break;
        case S_Matlab:
            lex = new QsciLexerMatlab;
            break;
        case S_Perl:
            lex = new QsciLexerPerl;
            break;
        case S_Python:
            lex = new QsciLexerPython;
            break;
        case S_Sql:
            lex = new QsciLexerSQL;
            break;
        case S_Tcl:
            lex = new QsciLexerTCL;
            break;
        case S_Tex:
            lex = new QsciLexerTeX;
            break;
        case S_Xml:
            lex = new QsciLexerXML;
            break;
        case S_Ruby:
            lex = new QsciLexerRuby;
            break;
        case S_JavaScript:
            lex = new QsciLexerJavaScript;
            break;
        default:
            lex = new QsciLexerPlain;
            break;
    }
    const auto c_text = g_pstyle->GetCodeTextColor();
    lex->setPaper(g_pstyle->GetCodeBgColor());
    lex->setDefaultPaper(g_pstyle->GetCodeBgColor());
    lex->setDefaultColor(c_text);
    auto f = qApp->font();
    f.setPointSize(10);
    lex->setFont(f);
    lex->setDefaultFont(f);

    // 有很多 role 都是黑色.
    QColor c_black;
    if (g_pstyle->GetCurStyle() == Qss_Dark) {
        for (int i = 0; i < 128; ++i) {
            c_black = lex->color(i);
            if (c_black.blue() == 0 && c_black.green() == 0
                && c_black.red() == 0) {
                lex->setColor(c_text, i);
            }
        }
    }

    return lex;
}

void CCodeWnd::UpdateNormalLayer()
{
    m_pwnd_normal->setLexer(GetLayer(g_data->sub_type));

    m_pwnd_normal->setMarginsForegroundColor(g_pstyle->GetCodeLineNumColor());
    m_pwnd_normal->setMarginsBackgroundColor(g_pstyle->GetCodeLineNumBgColor());

    const auto bg = g_pstyle->GetCodeBgColor();
    m_pwnd_normal->setFoldMarginColors(bg, bg);
    m_pwnd_normal->setEdgeColor(bg);
    m_pwnd_normal->setHotspotBackgroundColor(bg);

    m_pwnd_normal->setSelectionBackgroundColor(
        g_pstyle->GetCodeSelectionBgColor());
    m_pwnd_normal->setSelectionForegroundColor(
        g_pstyle->GetCodeSelectionForeColor());

    m_pwnd_normal->zoomTo(g_pini->GetIni(Ini::text_zoom_size_i).toInt());
}

void CCodeWnd::UpdateFormatLaxer(QsciScintilla *wnd, const FileSubType &t)
{
    wnd->setLexer(GetLayer(t));

    wnd->setMatchedBraceBackgroundColor(
        g_pstyle->GetCodeFormattedBraceBgColor());
    wnd->setMatchedBraceForegroundColor(
        g_pstyle->GetCodeFormattedBraceForeColor());
    wnd->setUnmatchedBraceBackgroundColor(
        g_pstyle->GetCodeFormattedUmmatchedBraceBgColor());
    wnd->setUnmatchedBraceForegroundColor(
        g_pstyle->GetCodeFormattedUmmatchedBraceForeColor());

    wnd->setCaretLineVisible(true);
    wnd->setCaretForegroundColor(g_pstyle->GetCodeFormattedCaretForeColor());
    wnd->setCaretLineBackgroundColor(g_pstyle->GetCodeFormattedCaretBgColor());

    wnd->setSelectionBackgroundColor(
        g_pstyle->GetCodeFormattedSelectionBgColor());
    wnd->setSelectionForegroundColor(
        g_pstyle->GetCodeFormattedSelectionForeColor());

    wnd->setFoldMarginColors(g_pstyle->GetCodeBgColor(),
                             g_pstyle->GetCodeBgColor());

    wnd->zoomTo(g_pini->GetIni(Ini::text_zoom_size_i).toInt());
}

void CCodeWnd::InitFormatWnd(QsciScintilla *wnd)
{
    wnd->setUtf8(true);
    wnd->setContextMenuPolicy(Qt::NoContextMenu);
    wnd->setReadOnly(true);
    wnd->setFrameStyle(QFrame::NoFrame);
    wnd->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    wnd->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    wnd->setWrapMode(QsciScintilla::WrapWord);
    wnd->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    wnd->setWrapVisualFlags(QsciScintilla::WrapFlagNone);

    // no margin
    wnd->setMarginWidth(1, 0);
    // fold
    wnd->setFolding(QsciScintilla::PlainFoldStyle);
}

void CCodeWnd::UpdateFormatLaxer(const FileSubType &t)
{
    UpdateFormatLaxer(m_pwnd_format, t);
}

QSize CCodeWnd::GetSize()
{
    const auto line_t     = m_pwnd_normal->lines();
    const auto factor     = g_pini->GetIni(Ini::text_zoom_size_i).toInt();
    const QFontMetrics fm = m_pwnd_normal->fontMetrics();
    QSize fm_sz           = fm.size(Qt::TextSingleLine, "M");
    if (factor == 0) {
        return QSize{fm_sz.width() * 90,
                     fm_sz.height() * line_t + line_t * fm.lineSpacing()};
    }

    QFont font = m_pwnd_normal->lexer() ? m_pwnd_normal->lexer()->defaultFont()
                                        : m_pwnd_normal->font();
    font.setPointSize(font.pointSize() + factor);
    return QSize{fm_sz.width() * 90, QFontMetrics(font).height() * line_t
                                         + line_t * fm.lineSpacing()};
}

bool CCodeWnd::eventFilter(QObject *obj, QEvent *ev)
{
    if ((obj == m_pwnd_normal->viewport() || obj == m_pwnd_format->viewport())
        && ev->type() == QEvent::Wheel) {
        QWheelEvent *e = (QWheelEvent *)ev;
        if (e->modifiers() == Qt::ControlModifier) {
            if (e->delta() > 0) {
                if (m_pwnd_normal->SendScintilla(m_pwnd_normal->SCI_GETZOOM)
                    == 20) {
                    return true;
                }
                m_pwnd_normal->zoomIn();
                m_pwnd_format->zoomIn();
            }
            else {
                if (m_pwnd_normal->SendScintilla(m_pwnd_normal->SCI_GETZOOM)
                    == -2) {
                    return true;
                }
                m_pwnd_normal->zoomOut();
                m_pwnd_format->zoomOut();
            }
            g_pini->SetIni(
                Ini::text_zoom_size_i,
                m_pwnd_normal->SendScintilla(m_pwnd_normal->SCI_GETZOOM));

            if (g_pini->GetIni(Ini::text_line_num_b).toBool()) {
                m_pwnd_normal->setMarginWidth(
                    1, GetNormalWndMarginWidth(m_pwnd_normal->lines()));
            }
            return true;
        }
    }
    return false;
}

int CCodeWnd::GetNormalWndMarginWidth(int line_counter)
{
    QFont font = m_pwnd_normal->lexer() ? m_pwnd_normal->lexer()->defaultFont()
                                        : m_pwnd_normal->font();
    font.setPointSize(font.pointSize()
                      + g_pini->GetIni(Ini::text_zoom_size_i).toInt());
    QFontMetrics fm(font);
    return fm.width(QString::number(line_counter)) + fm.xHeight() * 3;
}

#endif
