#include "browserview.h"
#include "loader.h"
#include <QMouseEvent>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWebHitTestResult>
#include <QMenu>

BrowserView::BrowserView(QWidget *parent) : //Constructor
    QWebView(parent)
{
    mainWindow = qobject_cast<browser*>(parent);
    isLoading = false;
    progress = 0;
    gesturePixel = 25; //The minimum pixels needed for the mouse to move with right click held down for it to register as a mouse gesture
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    page()->setForwardUnsupportedContent(true);
    connect(this, SIGNAL(linkClicked(QUrl)), SLOT(handleClickedLink(QUrl)));
}

void BrowserView::setInitialUrl(QUrl url) //Set m_initialUrl
{
    m_initialUrl = url;
}

void BrowserView::doGesture (QString gesture) //Handles mouse gesture functions.
{
    if (gesture == "up") {
        page()->triggerAction(QWebPage::ReloadAndBypassCache);
    }
    else if (gesture == "down") {
        reload();
    }
    else if (gesture == "left") {
        back();
    }
    else if (gesture == "left-up") {
    }
    else if (gesture == "left-down") {
    }
    else if (gesture == "right") {
        forward();
    }
    else if (gesture == "right-up") {

    }
    else if (gesture == "right-down") {

    }
}

QUrl BrowserView::initialUrl() //Return m_initialUrl
{
    return m_initialUrl;
}

void BrowserView::addLinkedTab()
{
    if (mainWindow == NULL)
            return;

    mainWindow->addTab(selectedLink, false);
}

void BrowserView::handleClickedLink(QUrl url)
{
    browser *mainWind  = qobject_cast<browser*>(window());
    if (mainWind->ctrlDown)
        mainWind->addTab(url.toString(), false);
    else
        setUrl(url);
}

void BrowserView::mousePressEvent(QMouseEvent *event) //On mouse press, handles mouse gesture
{
    gestureEnabled = false;
    gestureAccepted = false;

    if (event->button() == Qt::RightButton) {
            gestureEnabled = true;
           gestureStartPos = event->pos();
    }
    QWebView::mousePressEvent(event);
}

void BrowserView::mouseReleaseEvent(QMouseEvent *event) //On mouse release, for mouse gesture
{
    if (!this || !window() || isLoading) //It's possible for a gesture to attempt to start while a browserview is closing - this should prevent crashes
        return;
    gestureEnabled = false;
    if(gestureAccepted) {
        QPoint currLoc = event->pos();
        int diffY = currLoc.y() - gestureStartPos.y();
        int diffX = currLoc.x() - gestureStartPos.x();
        if (diffY < 0 && (diffX == 0 || abs(diffY / diffX) >= 2))
                doGesture("up");
        else if (diffY > 0 && (diffX == 0 || abs(diffY / diffX) >= 2))
                doGesture("down");
        else if (diffX < 0 && (diffY == 0 || abs(diffX / diffY) >= 2))
                doGesture("left");
        else if (diffX < 0 && diffY < 0 && abs(diffX / diffY) < 2 && abs((float)diffX / (float)diffY) > 0.5)
                doGesture("left-up");
        else if (diffX < 0 && diffY > 0 && abs(diffX / diffY) < 2 && abs((float)diffX / (float)diffY) > 0.5)
                doGesture("left-down");
        else if (diffX > 0 && (diffY == 0 || abs(diffX / diffY) >= 2))
                doGesture("right");
        else if (diffX > 0 && diffY < 0 && abs(diffX / diffY) < 2 && abs((float)diffX / (float)diffY) > 0.5)
                doGesture("right-up");
        else if (diffX > 0 && diffY > 0 && abs(diffX / diffY) < 2 && abs((float)diffX / (float)diffY) > 0.5)
                doGesture("right-down");
    }
    QWebView::mouseReleaseEvent(event);
}


void BrowserView::mouseMoveEvent(QMouseEvent *event) //On mouse move, handles mouse gesture
{

    if (gestureEnabled) {
        QPoint currLoc = event->pos();
        int diffY = currLoc.y() - gestureStartPos.x();
        int diffX = currLoc.x() - gestureStartPos.y();
        if (abs(diffY) >= gesturePixel || abs(diffX) >= gesturePixel)
            gestureAccepted = true;
    }
    QWebView::mouseMoveEvent(event);
}

void BrowserView::contextMenuEvent(QContextMenuEvent *event) //On right click [context menu showing] .. Modified right click menu
{
    if (!gestureAccepted) {
        QWebHitTestResult linkHovered(page()->mainFrame()->hitTestContent(event->pos()));
        if (!linkHovered.linkUrl().isEmpty()) {
                selectedLink = linkHovered.linkUrl().toString();
                QMenu menu(this);
                menu.addAction(pageAction(QWebPage::OpenLinkInNewWindow));
                menu.addAction(tr("Open in New Tab"), this, SLOT(addLinkedTab()));
                menu.addSeparator();
                menu.addAction(pageAction(QWebPage::DownloadLinkToDisk));
                menu.addSeparator();
                menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
                if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
                    menu.addAction(pageAction(QWebPage::InspectElement));
                menu.exec(mapToGlobal(event->pos()));
                return;
        }
        QWebView::contextMenuEvent(event);
    }
}

QWebView *BrowserView::createWindow(QWebPage::WebWindowType type) //Create new window
{
    Q_UNUSED(type);

    BrowserView *view = 0;
    browser *mainWind  = qobject_cast<browser*>(window());
    if (!(mainWind->ctrlDown)) {
        view = new BrowserView();
        mainWind->loader->newBrowser(false, view);
    }
    else {
        view = new BrowserView(mainWind);
        mainWind->addTab("Loading...", false, view);
    }
    return view;
}
