#include "cctrlsbarapp.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"

CCtrlsBarHelper::CCtrlsBarHelper()
{
    data = CCtrlsBarHelper::GetCtrlsDataList();
    foreach (const CtrlsData &i, data) {
        m_finder.insert(i.key.toLower(), i);
    }
}

CtrlsDataList CCtrlsBarHelper::GetExecFileList(const QString &suffix) const
{
    return m_finder.values(suffix.toLower());
}

CtrlsDataList CCtrlsBarHelper::GetCtrlsDataList()
{
    QList<CtrlsData> res;
    foreach (const QString &i, GetCtrlsData()) {
        const auto index = i.indexOf(g_ctrl_bar_separator);
        if (index == -1) {
            elprint;
            return {};
        }
        CtrlsData data;
        data.key = i.mid(0, index);

        const QString exe_args = i.mid(index + 1);
        const auto index_args  = exe_args.indexOf(g_ctrl_bar_separator_args);
        if (index_args == -1) {
            // no arguments, old version
            data.exe  = exe_args;
            data.args = "\"" + g_plugin_input_placeholder + "\"";
        }
        else {
            data.exe  = exe_args.mid(0, index_args);
            data.args = exe_args.mid(index_args + 1);
            if (data.args.isEmpty()) {
                data.args = "\"" + g_plugin_input_placeholder + "\"";
            }
        }
        res.append(data);
    }
    return res;
}

QStringList CCtrlsBarHelper::GetCtrlsData()
{
    return g_pini->GetIni(Ini::ctrls_app_list).toStringList();
}
