#ifndef QSCILEXERPLAIN_H
#define QSCILEXERPLAIN_H

#include <QObject>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexer.h>
class QsciLexerPlain : public QsciLexer {
public:
    QsciLexerPlain(QObject *parent = 0);

    virtual const char *language() const;
    virtual QString description(int style) const;
};

#endif  // QSCILEXERPLAIN_H
