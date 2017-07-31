#ifndef CCOMMANDHANDLER_H
#define CCOMMANDHANDLER_H

#include <QObject>
#include <QStringList>
#include "qcommandlineparser.h"

class CCommandHandler : public QObject {
    Q_OBJECT
public:
    explicit CCommandHandler(QObject* parent = nullptr);

    void Parse(const QStringList& m_args, bool is_duplicate_ins = true);

private:
    QCommandLineParser m_parser;

public:
signals:
    void SigHideWnd();
    void SigShakeWnd();
    void SigPreviewAFile(const QString& path);
    void SigDuplicatedIns();
    void SigShowTrayIcon();

public slots:
    void SlotNewInstanseActivated(const QStringList& args);
};

#endif  // CCOMMANDHANDLER_H
