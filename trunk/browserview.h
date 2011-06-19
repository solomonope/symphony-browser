#ifndef BROWSERVIEW_H
#define BROWSERVIEW_H

#include "tab.h"
#include "tabs.h"
#include "browser.h"
#include <QPoint>
#include <QWebView>
#include <QVariantList>
#include <QUrl>


class Tab;
class Tabs;
class browser;

class BrowserView : public QWebView
{
    Q_OBJECT

public:
        BrowserView(QWidget *parent = 0);
        void setInitialUrl(QUrl);
        void doGesture(QString gesture);
        QUrl initialUrl();
        browser *mainWindow;
        QString selectedLink;
        QPoint gestureStartPos;
        bool gestureEnabled;
        bool gestureAccepted;
        bool isLoading;
        int gesturePixel;
        int progress;

private:
        QUrl m_initialUrl;

private slots:
        void addLinkedTab();
        void handleClickedLink(QUrl);

protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void contextMenuEvent(QContextMenuEvent *event);
        QWebView *createWindow(QWebPage::WebWindowType type);
};

#endif // BROWSERVIEW_H
