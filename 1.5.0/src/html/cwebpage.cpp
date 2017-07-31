#include "cwebpage.h"
#include "stable.h"

#include <QAction>

CWebPage::CWebPage(QObject *parent) : QWebEnginePage(parent)
{
    for (int i = 0; i < QWebEnginePage::WebActionCount; ++i) {
        if (auto a = action(QWebEnginePage::WebAction(i))) {
            a->setEnabled(false);
        }
    }
}

bool CWebPage::acceptNavigationRequest(const QUrl &url,
                                       NavigationType type,
                                       bool isMainFrame)
{
    if (type == NavigationTypeLinkClicked) {
        emit SigLinkClicked(url);
        // has to  be false or
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

// void CWebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
// const QString& message, int lineNumber, const QString& sourceID)
//{
//    qprint << message << lineNumber << sourceID;
//    QWebEnginePage::javaScriptConsoleMessage(level, message, lineNumber,
//    sourceID);
//}

QWebEnginePage *CWebPage::createWindow(QWebEnginePage::WebWindowType /*type*/)
{
    return NULL;
}
