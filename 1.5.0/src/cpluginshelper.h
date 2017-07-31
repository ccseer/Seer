#ifndef CPLUGINSHELPER_H
#define CPLUGINSHELPER_H
#include "cplayprocess.h"

class CPluginsHelper : public QObject {
    Q_OBJECT
public:
    CPluginsHelper();
    ~CPluginsHelper();

    /*通过后缀名返回插件执行路径
    找到说明是插件,找不到说明不是插件并返回空
    */
    QString CheckPlugins(const QString& file);

    void Clear();

    bool LoadPlugin(const QString& path);

    bool IsFileFromPluginConverted(const QString& path);

private:
    void ClearLastPath();

    QString SplitExePathAndExeArgs(const QString& params);

    QString m_last_convert;
    QString m_last_source;

    QString m_input;
    QString m_output;

    CPlayProcess m_process;

signals:
    void SigError();

    void SigSucceed(const QString& new_path);

public slots:
    void Md5Ready(const QString& md5, const QString& target);

private slots:
    void ProcessDone(int code, QProcess::ExitStatus e);
};

#endif  // CPLUGINSHELPER_H
