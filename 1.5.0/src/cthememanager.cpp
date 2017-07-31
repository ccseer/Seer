#include "cthememanager.h"
#include "ccomponent.h"
#include "cglobaldata.h"
#include "settingshelper.h"
#include "stable.h"

#include <QFile>

CThemeManager::CThemeManager()
{
    m_style = QssStyle(g_pini->GetIni(Ini::theme_name_i).toInt());
}

QString CThemeManager::GetQssFilePath()
{
    return GetFileFromStyle(GetCurStyle());
}

QString CThemeManager::GetFileFromStyle(QssStyle s)
{
    QString name;
    switch (s) {
        case Qss_Dark:
            name = "://dark.qss";
            break;
        default:
            name = "://default.qss";
            break;
    }
    return name;
}

QColor CThemeManager::GetAppTitleBarShadowColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "black";
        default:
            return "#333333";
    }
}

QString CThemeManager::GetAppLoadingPath()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "://loading_dark.gif";
        default:
            return "://loading.gif";
    }
}

QColor CThemeManager::GetInfoDynamicRowBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "transparent";
        default:
            return "#fafafa";
    }
}

QColor CThemeManager::GetCodeBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#323232";
        default:
            return "white";
    }
}

QColor CThemeManager::GetCodeTextColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "white";
        default:
            return "#333333";
    }
}

QColor CThemeManager::GetCodeLineNumColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#5a5a5a";
        default:
            return "#808080";
    }
}

QColor CThemeManager::GetCodeLineNumBgColor()
{
    return GetCodeBgColor();
}

QColor CThemeManager::GetCodeSelectionBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#264f78";
        default:
            return "#c2daff";
    }
}

QColor CThemeManager::GetCodeSelectionForeColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#cccccc";
        default:
            return "#343434";
    }
}

QColor CThemeManager::GetCodeFormattedBraceBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#113d6f";
        default:
            return GetCodeBgColor();
    }
}

QColor CThemeManager::GetCodeFormattedBraceForeColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#f2ac49";
        default:
            return "#b37e36";
    }
}

QColor CThemeManager::GetCodeFormattedUmmatchedBraceBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return GetCodeFormattedBraceBgColor();
        default:
            return GetCodeBgColor();
    }
}

QColor CThemeManager::GetCodeFormattedUmmatchedBraceForeColor()
{
    return "red";
}

QColor CThemeManager::GetCodeFormattedCaretForeColor()
{
    // 按下 insert 之后光标的颜色
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "black";
        default:
            return "#333333";
    }
}

QColor CThemeManager::GetCodeFormattedCaretBgColor()
{
    // 当前行背景色
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#3d3d3d";
        default:
            return GetCodeSelectionBgColor().lighter(120);
    }
}

QColor CThemeManager::GetCodeFormattedSelectionBgColor()
{
    return GetCodeSelectionBgColor();
}

QColor CThemeManager::GetCodeFormattedSelectionForeColor()
{
    return GetCodeSelectionForeColor();
}

QString CThemeManager::GetHtmlMarkdownPath()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "://html/css/md_night.css";
        default:
            return "://html/css/md_github.css";
    }
}

QColor CThemeManager::GetSceneBackgroundColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#323232";
        default:
            return "white";
    }
}

QColor CThemeManager::GetMediaBgColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "black";
        default:
            return "white";
    }
}

QColor CThemeManager::GetSettingsThemeRectColor()
{
    switch (GetCurStyle()) {
        case Qss_Dark:
            return "#47c4f9";
        default:
            return "#0288d1";
    }
}

void CThemeManager::SetCurStyle(QssStyle s)
{
    g_pini->SetIni(Ini::theme_name_i, (int)s);
}

QssStyle CThemeManager::GetCurStyle() const
{
    return m_style;
}
