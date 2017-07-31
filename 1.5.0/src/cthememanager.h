#ifndef CTHEMEMANAGER_H
#define CTHEMEMANAGER_H

#include "csingleton.h"
#include "types.h"

#include <QColor>
#include <QPixmap>

#define g_pstyle CSingleton<CThemeManager>::Instance()
class CThemeManager {
public:
    CThemeManager();

    QssStyle GetCurStyle() const;
    void SetCurStyle(QssStyle s);
    QString GetQssFilePath();

    // app
    QColor GetAppTitleBarShadowColor();
    QString GetAppLoadingPath();


    // info
    QColor GetInfoDynamicRowBgColor();

    // code
    QColor GetCodeBgColor();
    QColor GetCodeTextColor();
    QColor GetCodeLineNumColor();
    QColor GetCodeLineNumBgColor();
    QColor GetCodeSelectionBgColor();
    QColor GetCodeSelectionForeColor();

    QColor GetCodeFormattedBraceBgColor();
    QColor GetCodeFormattedBraceForeColor();
    QColor GetCodeFormattedUmmatchedBraceBgColor();
    QColor GetCodeFormattedUmmatchedBraceForeColor();
    QColor GetCodeFormattedCaretForeColor();
    QColor GetCodeFormattedCaretBgColor();
    QColor GetCodeFormattedSelectionBgColor();
    QColor GetCodeFormattedSelectionForeColor();

    // html
    QString GetHtmlMarkdownPath();

    // pdf & image
    QColor GetSceneBackgroundColor();

    // media
    QColor GetMediaBgColor();

    // settings
    QColor GetSettingsThemeRectColor();

private:
    QString GetFileFromStyle(QssStyle s);

    QssStyle m_style;
};

#endif  // CTHEMEMANAGER_H
