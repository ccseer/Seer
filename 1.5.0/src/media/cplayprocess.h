#ifndef CPLAYPROCESS_H
#define CPLAYPROCESS_H

#include <QProcess>

class CPlayProcess : public QProcess {
    Q_OBJECT
public:
    explicit CPlayProcess(QObject* parent = 0);
    ~CPlayProcess();
    void kill();
    void start(const QString& program,
               const QStringList& arguments,
               OpenMode mode = ReadWrite);
    void start(const QString& program);
    void start(OpenMode mode = ReadWrite);

    bool IsFinishedByKilled() const
    {
        return m_is_killed;
    }

    bool StartProcess(const QString& exe_path, const QStringList& args);

private:
    bool m_is_killed;

public:
signals:
    void SigFinishedNaturally();
    void SigFinishedByKilled();

public slots:
    bool IsRunning();
    void QuitProcess();
    void Command(const QString& str);
};

#endif  // CPLAYPROCESS_H
