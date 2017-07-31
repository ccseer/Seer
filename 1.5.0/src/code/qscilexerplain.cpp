#include "qscilexerplain.h"

QsciLexerPlain::QsciLexerPlain(QObject *parent) : QsciLexer(parent)
{
}

const char *QsciLexerPlain::language() const
{
    return "plain";
}

QString QsciLexerPlain::description(int style) const
{
    Q_UNUSED(style);
    return "Plain Text";
}
