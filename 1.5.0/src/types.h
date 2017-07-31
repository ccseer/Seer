#ifndef TYPES
#define TYPES

#include <QList>

/*************** types  *********************/

struct FileProperty {
    QString key;
    QString value;
};
using FilePropertyData = QList<FileProperty>;

// Plugin
enum PluginColumn {
    PC_Name = 0,
    PC_Suffix,
    PC_Para,
};
struct PluginData {
    QString name;
    QStringList suffix;
    QString para;
};
using PluginsList = QList<PluginData>;

// Ctrl bar
struct CtrlsData {
    QString key;
    QString exe;
    QString args;
};
using CtrlsDataList = QList<CtrlsData>;

// qss
enum QssStyle {
    Qss_Default = 0,
    Qss_Dark,
};

// type
enum DisplayType {
    None,
    Html,
    Pic,
    Media,
    Dir,
    Pdf,
    Code,
};

enum FileSubType {
    S_Unknown,

    S_Video,
    S_Audio,

    S_Movie,
    S_Psd,
    S_Pics,

    S_Mrakdown,
    S_Web,

    S_Bash,
    S_Batch,
    S_Cmake,
    S_C_Shap,
    S_Cpp,
    S_Css,
    S_Diff,
    S_Fortran77,
    S_Fortran90,
    S_Html,
    S_Java,
    S_JavaScript,
    S_Lua,
    S_Matlab,
    S_Perl,
    S_Python,
    S_Sql,
    S_Tcl,
    S_Tex,
    S_Ruby,
    S_Xml,
    S_Txt,

    S_Rar,
    S_Zip,
    S_Folder,

    S_PDF,
};

#endif  // TYPES
