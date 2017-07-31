#ifndef CWEBENGINEPAGE_H
#define CWEBENGINEPAGE_H

#include <QWebEnginePage>

class CWebPage : public QWebEnginePage {
    Q_OBJECT

public:
    explicit CWebPage(QObject *parent = 0);
    //~CWebPage();

protected:
    //    virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel
    //    level, const QString& message, int lineNumber, const QString&
    //    sourceID);

    virtual QWebEnginePage *createWindow(WebWindowType type);
    virtual bool acceptNavigationRequest(const QUrl &url,
                                         NavigationType type,
                                         bool isMainFrame);

private:
signals:
    void SigLinkClicked(const QUrl &url);
};

#endif  // CWEBENGINEPAGE_H
