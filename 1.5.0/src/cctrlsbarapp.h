#ifndef CCTRLSBARAPP_H
#define CCTRLSBARAPP_H

#include <QStringList>
#include "csettingswnd4controls.h"

#define g_ctrl_bar_separator ":"
#define g_ctrl_bar_separator_args "?"

class CCtrlsBarHelper {
public:
    explicit CCtrlsBarHelper();
    CtrlsDataList GetExecFileList(const QString &suffix) const;

    CtrlsDataList data;

private:
    static CtrlsDataList GetCtrlsDataList();
    static QStringList GetCtrlsData();

    QMultiHash<QString, CtrlsData> m_finder;
};

#endif  // CCTRLSBARAPP_H
