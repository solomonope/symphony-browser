#include "browser.h"
#include "ui_browser.h"
#include "ui_finddialog.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QMessageBox>
#include <QKeyEvent>
#include <QToolBar>
#include <QMenu>
#include <QWebFrame>
#include <QFile>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QSettings>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSignalMapper>
#include <QWebHistory>
#include <QUrlInfo>
#include <QWebHistoryItem>
#include <QtAlgorithms>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QHostInfo>
#include <QStringBuilder>
#include <QSslError>
#include "adblockpage.h"
#include "loader.h"

bool* browser::m_adBlockEnabled = 0;
bool* browser::m_adBlock2Enabled = 0;
AdBlockSubscription* browser::m_subscrip = 0;


browser::browser(QWidget *parent, Loader *v_loader) : //Constructor. Most of the fun stuff happens in doLoad method
    QMainWindow(parent),
    ui(new Ui::browser),
    findDialog(new Ui::findDialog)
{
    ui->setupUi(this);
    loader = v_loader;
}
browser::~browser() //Destructor.
{
    delete ui;
}

void browser::doLoad() //Handles all functions that occur on browser launch
{
    ui->topBar->addWidget(ui->backBtn);
    ui->topBar->addWidget(ui->refreshBtn);
    ui->topBar->addWidget(ui->fwdBtn);
    ui->topBar->addSeparator();
    ui->topBar->addWidget(ui->mainBtn);
    ui->topBar->addSeparator();
    ui->topBar->addWidget(ui->homeBtn);
    ui->topBar->addSeparator();
    ui->topBar->addWidget(ui->bookmarkBtn);
    ui->topBar->addSeparator();
    ui->topBar->addWidget(ui->webIcon);
    ui->topBar->addWidget(ui->addressBar);
    ui->topBar->addWidget(ui->addBookmark);
    ui->topBar->addWidget(ui->goBtn);

    ui->topBar->addSeparator();
    ui->topBar->addWidget(ui->pluginBar);

    noScript->isEnabled = noScriptEnabled;
    if (*noScriptEnabled)
        ui->pluginBar->addWidget(ui->noScriptBtn);
    else {
        ui->noScriptBtn->setParent(0);
        ui->noScriptBtn->hide();
    }

    setMode(*mode);

    connect(accessMgr, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(handleSSL(QNetworkReply*,QList<QSslError>)));

    completer = new Completer(this, history->getUrlList(), ui->addressBar);

    tabs = new Tabs(this);
    tabBar = new TabBar(this, tabs, ui->addTabBtn);
    tabs->tabBar = tabBar;
    connect(tabs, SIGNAL(widgetRemoved(int)), SLOT(updateTabSession(int)));
    int topFiller = ui->topBar->height(); //Put extra height accumulated from all controls above this one
    int bottomFiller = ui->statusBar->height(); //Put extra height accumulated from all controls below this one
    tabBar->setGeometry(0, 0 + topFiller, geometry().width(), 30);
    tabs->setGeometry(0, 0 + topFiller + 30, geometry().width(), geometry().height() - topFiller - bottomFiller);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tab_changed(int)));

    ui->statusBar->setGeometry(0, geometry().height() - ui->statusBar->height(), geometry().width() - 2, ui->statusBar->height());

    ui->statusBtn->setGeometry(1, ui->statusBar->y() + 1, ui->statusBtn->width(), ui->statusBar->height() - 2);
    ui->statusBtn->raise();

    QPalette tmpPal = ui->statusBar->palette();
    tmpPal.setColor(QPalette::Base, palette().window().color());
    ui->statusBar->setPalette(tmpPal);

    QString statusStylesheet = "border: 0;";
    QString statusBackColor = palette().window().color().name();
    statusStylesheet.append(" background-color: " % statusBackColor % ";");
    ui->statusBar->setStyleSheet(statusStylesheet);

    QSettings settings("data/settings.dat", QSettings::IniFormat);

    globalPrivateBrowsing = false;

    ctrlDown = false;
    shiftDown = false;
    inspectEnabled = false;
    modificationEnabled = false;

    updateWhitelist();

    if (doPreloadTabs) {
        if (settings.value("OpenMode").toInt() == 0)
            addTab(*homePage, false);
        else {
            QStringList tabSession = settings.value("TabSession").toStringList();
            if (tabSession.count() == 0)
                addTab(*homePage, false);
            else {
                for (int i = tabSession.count() - 1; i >= 0; i--)
                    addTab(tabSession.at(i), false);
            }
        }
        tabs->setCurrentIndex(0);
    }
    else {
        if (initView)
            addTab("Loading...", false, initView);
    }

    prefWind = new Preferences(this);

    //Find window
    findWind = new QDialog(this);
    findDialog->setupUi(findWind);
    QPushButton *findBtn = new QPushButton("Find");
    findBtn->setDefault(true);
    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setAutoDefault(false);
    findDialog->buttonBox->addButton(findBtn, QDialogButtonBox::AcceptRole);
    findDialog->buttonBox->addButton(closeBtn, QDialogButtonBox::RejectRole);
    connect(findDialog->buttonBox, SIGNAL(accepted()), this, SLOT(doFind()));
    connect(findDialog->buttonBox, SIGNAL(rejected()), this, SLOT(closeFind()));
    connect(findWind, SIGNAL(rejected()), SLOT(closeFind()));

    //Togglable QActions [for menus]
    privBrowsing = new QAction("Enable Private Browsing", this);
    privBrowsing->setCheckable(true);
    siteInspection = new QAction("Enable Site Inspection", this);
    siteInspection->setCheckable(true);
    contentMod = new QAction("Enable Content Modification", this);
    contentMod->setCheckable(true);
    connect(contentMod, SIGNAL(triggered()), SLOT(enableModification()));
    connect(siteInspection, SIGNAL(triggered()), SLOT(enableInspect()));
    connect(privBrowsing, SIGNAL(triggered()), SLOT(togglePrivateBrowsing()));

    loadNoScriptWind();

    show();

    tabs->tabBar->refreshTabScrollers();
}

void browser::attemptBypassAdblock(QUrl badUrl)
{
    if (badUrl.isEmpty() || !(badUrl.isValid()) || badUrl.host().isEmpty())
        return;
    QString host = badUrl.host();
    QMessageBox *warning = new QMessageBox(this);
    warning->setIcon(QMessageBox::Warning);
    QPushButton *yesBtn = warning->addButton("Yes - Turn off Adblock for " % host, QMessageBox::YesRole);
    warning->addButton("No - Cancel", QMessageBox::NoRole);
    warning->setText("AdBlock may be the cause of " % badUrl.toString() % " failing to load.\nWould you like to whitelist all of " % host % " - so AdBlock won't affect it anymore?");
    warning->exec();
    if (warning->clickedButton() == yesBtn) {
        QSettings settings("data/whitelist.dat", QSettings::IniFormat);
        QStringList whitelist;
        if (settings.contains("adblock"))
            whitelist = settings.value("adblock").toStringList();
        whitelist.append(host);
        settings.setValue("adblock", whitelist);
        subscrip()->updateWhitelist();
        doNavigate(badUrl.toString());
    }
}

void browser::setSubscrip(AdBlockSubscription *sub)
{
    m_subscrip = sub;
}

void browser::setAdBlockEnabled(bool *b)
{
    m_adBlockEnabled = b;
}
void browser::setAdBlock2Enabled(bool *b)
{
    m_adBlock2Enabled = b;
}

void browser::setNoScriptEnabled(bool *b)
{
    *noScriptEnabled = *b;
    if (!(*noScript->isEnabled == *b))
        *noScript->isEnabled = *b;
    if (!(*b)) {
        ui->noScriptBtn->setParent(0);
        ui->noScriptBtn->hide();
    }
    else {
        ui->noScriptBtn->show();
        ui->noScriptBtn->setParent(ui->pluginBar);
        ui->pluginBar->addWidget(ui->noScriptBtn);
    }
}

void browser::handleNoScript()
{
    noScript->scriptsCurr.clear();

    QString host = tabs->getCurrBrowser()->url().host();

    if (host.startsWith("www."))
        host = host.replace("www.", "");

    int status = noScript->checkStatus(host);
    bool allowed = true;
    if ((status == 2 && noScript->defaultSecure == true) || status == 0)
        allowed = false;
    if (allowed == false) {
        tabs->getCurrBrowser()->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        tabs->getCurrBrowser()->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    }
    else {
        tabs->getCurrBrowser()->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
        tabs->getCurrBrowser()->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    }
}

void browser::updateNoScriptSecure()
{
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    noScript->defaultSecure = false;
    if (settings.contains("NoScriptSecureEnabled")) {
        if (settings.value("NoScriptSecureEnabled").toInt() == 1)
            noScript->defaultSecure = true;
    }
}

void browser::loadNoScriptWind()
{
    noScriptWind = new QMainWindow(this);
    noScriptWind->setGeometry(geometry().x(), geometry().y() + 75, 400, 450);
    noScriptWind->setMaximumWidth(400);
    noScriptWind->resize(400, 450);
    noScriptWind->setWindowTitle("Manage NoScript - Symphony");
    noScriptView = new QTableView(noScriptWind);

    noScriptList = new QStandardItemModel(noScriptWind);

    noScriptList->insertColumn(0);
    noScriptList->insertColumn(1);
    noScriptList->setHeaderData(0, Qt::Horizontal, "Url", Qt::DisplayRole);
    noScriptList->setHeaderData(1, Qt::Horizontal, "Javascript & Plugins", Qt::DisplayRole);

    noScriptView->setModel(noScriptList);
    noScriptView->setEditTriggers(NULL);

    connect(noScriptView, SIGNAL(doubleClicked(QModelIndex)), SLOT(click_noScriptWind(QModelIndex)));

    noScriptWind->setCentralWidget(noScriptView);
}


void browser::updateWhitelist()
{
    whitelistON = QStringList();
    whitelistOFF = QStringList();
    QSettings settings("data/whitelist.dat", QSettings::IniFormat);
    if (settings.contains("siteON"))
        whitelistON = settings.value("siteON").toStringList();
    if (settings.contains("siteOFF"))
        whitelistOFF = settings.value("siteOFF").toStringList();
}

void browser::addTab(QString urlStr, bool privateBrowsing) //Add a tab with a specified URL; Title defaults to "New Tab"
{
    if (QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        privateBrowsing = true;

    //Set up base vars for new tab...
    Tab *newTab = new Tab(tabs);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    BrowserView *newWeb = new BrowserView();
    verticalLayout->addWidget(newWeb);
    verticalLayout->setMargin(0);
    newTab->setLayout(verticalLayout);

    newWeb->settings()->setIconDatabasePath("data"); //This takes a bit of time to load, so stick it near the top of this function

    //Set up events

    connect(newWeb->page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(link_hovered(QString,QString,QString)));
    connect(newWeb->page(), SIGNAL(windowCloseRequested()), SLOT(handleCloseWindow()));
    connect(newWeb, SIGNAL(titleChanged(QString)), SLOT(title_changed(QString)));
    connect(newWeb, SIGNAL(loadStarted()), SLOT(load_start()));
    connect(newWeb, SIGNAL(loadFinished(bool)), SLOT(load_finish(bool)));
    connect(newWeb, SIGNAL(loadFinished(bool)), SLOT(update_history(bool)));
    connect(newWeb, SIGNAL(loadProgress(int)), SLOT(load_progress(int)));
    connect(newWeb, SIGNAL(statusBarMessage(QString)), SLOT(update_status(QString)));
    connect(newWeb, SIGNAL(linkClicked(QUrl)), SLOT(handleLinkClicked(QUrl)));
    connect(newWeb, SIGNAL(urlChanged(QUrl)), SLOT(changeAddressBar(QUrl)));

    if (privateBrowsing) {
        newWeb->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
        newTab->privateBrowsing = true;
    }
    else
        newTab->privateBrowsing = false;

    newWeb->page()->setNetworkAccessManager(accessMgr);

    tabs->addTab(newTab);
    tabs->setTabText(tabs->count() - 1, "New Tab");

    if (modificationEnabled)
        newWeb->page()->setContentEditable(true);
    if (inspectEnabled)
        newWeb->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled , true);

    if (urlStr != NULL)
        newWeb->setUrl(QUrl(urlStr)); //Navigate to URL

    tabs->setCurrentIndex(tabs->count() - 1);
}



void browser::addTab(QString theTitle, bool privateBrowsing, BrowserView *newWeb) //Add a tab with a specified URL; Title defaults to "New Tab"
{
    if (QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        privateBrowsing = true;


    //Set up base vars for new tab...
    Tab *newTab = new Tab(tabs);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setMargin(0);
    verticalLayout->addWidget(newWeb);
    newTab->setLayout(verticalLayout);

    newWeb->settings()->setIconDatabasePath("data"); //This takes a bit of time to load, so stick it near the top of this function

    //Set up events

    connect(newWeb->page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(link_hovered(QString,QString,QString)));
    connect(newWeb->page(), SIGNAL(windowCloseRequested()), SLOT(handleCloseWindow()));
    connect(newWeb, SIGNAL(titleChanged(QString)), SLOT(title_changed(QString)));
    connect(newWeb, SIGNAL(loadStarted()), SLOT(load_start()));
    connect(newWeb, SIGNAL(loadFinished(bool)), SLOT(load_finish(bool)));
    connect(newWeb, SIGNAL(loadFinished(bool)), SLOT(update_history(bool)));
    connect(newWeb, SIGNAL(loadProgress(int)), SLOT(load_progress(int)));
    connect(newWeb, SIGNAL(statusBarMessage(QString)), SLOT(update_status(QString)));
    connect(newWeb, SIGNAL(linkClicked(QUrl)), SLOT(handleLinkClicked(QUrl)));
    connect(newWeb, SIGNAL(urlChanged(QUrl)), SLOT(changeAddressBar(QUrl)));

    if (privateBrowsing) {
        newWeb->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
        newTab->privateBrowsing = true;
    }
    else
        newTab->privateBrowsing = false;

    newWeb->page()->setNetworkAccessManager(accessMgr);


    tabs->addTab(newTab);
    tabs->setTabText(tabs->count() - 1, theTitle);

    if (modificationEnabled)
        newWeb->page()->setContentEditable(true);
    if (inspectEnabled)
        newWeb->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled , true);

    tabs->setCurrentIndex(tabs->count() - 1);
}

void browser::saveTabs() //Saves all tabs to publicTabHistory. Used when toggling private browsing on.
{
    for(int i = 0; i < tabs->count(); i++)
        publicTabHistory.insert(0, tabs->getTab(i)->getBrowser()->url().toString());

    tabs->clear();
}

void browser::restoreTabs() //Restores all public tabs. Used when toggling private browsing off.
{
    tabs->clear();
    for (int i = publicTabHistory.count() - 1; i >= 0; i--) {
        addTab(publicTabHistory.at(i), false);
        publicTabHistory.removeAt(i);
    }
}

void browser::setStatusTxt(QString txt) //Sets the status bar text, or tooltip text, depending on what status is set to.
{
    ui->statusBar->setText("         " % txt);
}

void browser::addBookmark(QString url, QString title) //Adds bookmark
{
    bookmarks->addItem(title, url);
    alert("Bookmarked added successfully.");
}

void browser::doNavigate(QString url) //Handles navigating to a website.
{
    if (ui->addressBar->text().isEmpty() && url.isEmpty())
        return;

    int curr = tabs->currentIndex();
    if (tabs->count() == 0) {
        addTab(NULL, false);
        curr = 0;
    }

    QString origStr = ui->addressBar->text();
    if (!(url.isEmpty()))
        origStr = url;

    if (QUrl(origStr).scheme() == QLatin1String("javascript")) {
        tabs->getCurrBrowser()->page()->mainFrame()->evaluateJavaScript(QUrl::fromPercentEncoding(QUrl(origStr).toString(Q_FLAGS(QUrl::TolerantMode|QUrl::RemoveScheme)).toAscii()));
        return;
    }

    QByteArray origBA(origStr.toUtf8());
    QUrl formattedUrl = QUrl::fromUserInput(QUrl::fromEncoded(origBA).toString());

    tabs->getCurrBrowser()->setInitialUrl(formattedUrl);
    setStatusTxt("Resolving...");
    tabs->setTabText(tabs->currentIndex(), "Loading...");
    QHostInfo::lookupHost(formattedUrl.host(), this, SLOT(finishNavigating(QHostInfo))); //Workaround for QtWebkit bug as seen on: https://bugs.webkit.org/show_bug.cgi?id=61328
}

void browser::alert(QString str) //Turns str in to a basic QMessageBox
{
    QMessageBox msg;
    msg.setWindowTitle("Alert - Symphony");
    msg.setText(str);
    msg.exec();
}

void browser::saveTabSession() //Saves all non-private tabs. Occurs on close.
{
    QStringList tmp;
    for (int i = 0; i < tabs->count(); i++) {
        Tab *tmpTab = tabs->getTab(i);
        if (!(tmpTab->privateBrowsing))
            tmp.insert(0, tmpTab->getBrowser()->url().toString());
    }
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    settings.setValue("TabSession", tmp);
}

void browser::newWindow() //Creates new browser window.
{
    loader->newBrowser(true);
}

AdBlockSubscription* browser::subscrip()
{
    return m_subscrip;
}

bool* browser::adBlockEnabled()
{
    return m_adBlockEnabled;
}

bool* browser::adBlock2Enabled()
{
    return m_adBlock2Enabled;
}

void browser::disableAdBlockSite()
{
    QUrl url(tabs->getCurrBrowser()->url());
    QString host = url.host();
    QMessageBox *warning = new QMessageBox(this);
    warning->setIcon(QMessageBox::Warning);
    QPushButton *yesBtn = warning->addButton("Yes - Turn Off AdBlock For " % host, QMessageBox::YesRole);
    warning->addButton("No - Cancel", QMessageBox::NoRole);

    warning->setText("\nWould you like to whitelist all of " % host % " - so AdBlock won't affect it anymore?");
    warning->exec();
    if (warning->clickedButton() == yesBtn) {
        QSettings settings("data/whitelist.dat", QSettings::IniFormat);
        QStringList whitelist;
        if (settings.contains("adblock"))
            whitelist = settings.value("adblock").toStringList();
        whitelist.append(host);
        settings.setValue("adblock", whitelist);
        subscrip()->updateWhitelist();
        tabs->getCurrBrowser()->reload();
        alert("AdBlock has been disabled for " % host % ".");
    }

}

void browser::refreshNoScriptList()
{
    noScriptList->removeRows(0, noScriptList->rowCount());

    noScriptView->setColumnWidth(0, 262);
    noScriptView->setColumnWidth(1, 120);

    QHash<QString, bool> noScriptUrls = noScript->getHosts();

    for (int i = 0; i < noScriptUrls.keys().count(); i++) {
    QString column2 = "Off";
    QString column1 = noScriptUrls.keys().at(i);
    bool enabled = noScriptUrls.values().at(i);
    if (enabled)
        column2 = "On";
    noScriptList->insertRow(noScriptList->rowCount());

    noScriptList->setData(noScriptList->index(i, 0), column1, Qt::DisplayRole);
    noScriptList->setData(noScriptList->index(i, 1), column2, Qt::DisplayRole);
    }
}

void browser::click_noScriptWind(QModelIndex index)
{
    QString host;
    bool enabled;
    if (index.column() == 0)
        host = index.data().toString();
    else
        host = noScriptList->itemData(noScriptList->index(index.row(), 0)).value(0).toString();

    if (index.column() == 1) {
        if (index.data().toString() == "On")
            enabled = true;
        else
            enabled = false;
    }
    else {
        if (noScriptList->itemData(noScriptList->index(index.row(), 1)).value(0).toString() == "On")
            enabled = true;
        else
            enabled = false;
    }


    QString tmp = "off";
    if (enabled)
        tmp = "on";
    QString tmp2 = "on";
    if (enabled)
        tmp2 = "off";


    QMessageBox msgBox;
    QPushButton *yesBtn = msgBox.addButton("Yes", QMessageBox::YesRole);
    msgBox.addButton("No", QMessageBox::NoRole);
    msgBox.setText("Javascript for " % host % " is currently toggled " % tmp % ".\nWould you like to toggle javascript for " % host % " " % tmp2 % "?");
    msgBox.exec();
    if (msgBox.clickedButton() == yesBtn) {
        if (enabled)
            noScript->blockHost(host, true);
        else
            noScript->blockHost(host, false);
        tabs->getCurrBrowser()->reload();
    }

    refreshNoScriptList();
}

void browser::togglePopupPerm()
{
    QString host = tabs->getCurrBrowser()->url().host();
    QMessageBox *warning = new QMessageBox(this);
    warning->setIcon(QMessageBox::Warning);
    QPushButton *onBtn = warning->addButton("Toggle Always [ON] for " % host, QMessageBox::YesRole);
    QPushButton *offBtn = warning->addButton("Toggle Always [OFF] for" % host, QMessageBox::NoRole);
    warning->addButton("Cancel", QMessageBox::RejectRole);

    warning->setText("Would you like to toggle popups always enabled/disabled for " % host % "?");
    warning->exec();
    if (warning->clickedButton() == onBtn) {
        QSettings settings("data/whitelist.dat", QSettings::IniFormat);
        QStringList whitelist;
        if (settings.contains("siteON"))
            whitelist = settings.value("siteON").toStringList();
        whitelist.append(host);
        settings.setValue("siteON", whitelist);
        QStringList whitelist2;
        if (settings.contains("siteOFF"))
            whitelist2 = settings.value("siteOFF").toStringList();
        if (whitelist2.contains(host)) {
            whitelist2.removeOne(host);
            settings.setValue("siteOFF", whitelist2);
        }
        updateWhitelist();
        tabs->getCurrBrowser()->reload();
        alert("Popups for " % host % " have been toggled always on.");
    }
    else if(warning->clickedButton() == offBtn) {
            QSettings settings("data/whitelist.dat", QSettings::IniFormat);
            QStringList whitelist;
            if (settings.contains("siteOFF"))
                whitelist = settings.value("siteOFF").toStringList();
            whitelist.append(host);
            settings.setValue("siteOFF", whitelist);
            QStringList whitelist2;
            if (settings.contains("siteON"))
                whitelist2 = settings.value("siteON").toStringList();
            if (whitelist2.contains(host)) {
                whitelist2.removeOne(host);
                settings.setValue("siteON", whitelist2);
            }
            updateWhitelist();
            tabs->getCurrBrowser()->reload();
            alert("Popups for " % host % " have been toggled always off.");
    }
}

void browser::handleSSL(QNetworkReply* reply, QList<QSslError> error) //Handles all SSL errors
{

    QString host = reply->url().host();

    if (sslWhiteList) {
        if (sslWhiteList->contains(host)) {
            reply->ignoreSslErrors();
            return;
        }
    }

    QStringList errorStrings;
    for (int i = 0; i < error.count(); ++i)
        errorStrings += error.at(i).errorString();

    if (errorStrings.isEmpty()) {
        reply->ignoreSslErrors();
        return;
    }

    QString errors = errorStrings.join(QLatin1String("</li><li>"));
    QMessageBox *warning = new QMessageBox(this);
    warning->setIcon(QMessageBox::Warning);
    QPushButton *neverBtn = warning->addButton("Never Tell Me For This Site", QMessageBox::AcceptRole);
    QPushButton *ignoreBtn = warning->addButton("Ignore Once", QMessageBox::YesRole);
    warning->addButton("Cancel", QMessageBox::NoRole);

    //Need to recode the warning's text a bit, as it is a near-exact duplicate of Arora's at the moment.
    warning->setText("<qt>SSL Errors:"
                     "<br/><br/>for: <tt>" % reply->url().toString() % "</tt>"
                     "<ul><li>" % errors % "</li></ul>\n\n"
                     "What do you want to do?</qt>");
    warning->exec();

    if (warning->clickedButton() == ignoreBtn)
        reply->ignoreSslErrors();
    else if (warning->clickedButton() == neverBtn) {
        if (sslWhiteList) {
            sslWhiteList->insert(0, host);
            QSettings settingsWL("data/whitelist.dat", QSettings::IniFormat);
            settingsWL.setValue("ssl", *sslWhiteList);
        }
        reply->ignoreSslErrors();
    }
}

void browser::setMode() //Asks user which mode they want to use.
{
    QString modeDescription;
    QString currMode;
    switch (*mode) {
    case 0:
        currMode = "Level 1";
    break;
    case 1:
        currMode = "Level 2";
    break;
    case 2:
        currMode = "Level 3";
    break;
    case 3:
        currMode = "Level 4";
    break;
    }
    modeDescription.append("Your current security mode is set to: " % currMode % ".");
    modeDescription.append("\nNote that all of your currently open tabs will be refreshed if you change your mode.");
    modeDescription.append("\n\nLevel 1:\nThis mode enables all features that the browser has to offer.\nIt also allows web sites to have the most control over this browser. Although this mode will give you the most powerful browsing experience, your privacy may be at risk if you go to a 'bad' website.");
    modeDescription.append("\n\nLevel 2 - Recommended:\nThis mode enables most features that the browser has to offer.\nIt is very similar to Extra Powerful, except that there are a few javascript features that are disabled while you are in this mode.");
    modeDescription.append("\n\nLevel 3:\nThis mode does not enable a lot of the features of this browser.\nWhile your privacy and computer are quite a bit more secure while you are in this mode, some websites may not display correctly, or at all.");
    modeDescription.append("\n\nLevel 4:\nThis mode offers the most security for you.\nBut, this mode disables a lot of this browser's features. A lot of websites may not display correctly or at all while in this mode.");
    modeDescription.append("\n");
    QMessageBox ask;
    ask.setWindowTitle("Set Symphony Security Mode");
    ask.setText(modeDescription);
    QPushButton *btn0 = ask.addButton(QString("Choose: Level 1"), QMessageBox::AcceptRole);
    QPushButton *btn1 = ask.addButton(QString("Choose: Level 2"), QMessageBox::AcceptRole);
    QPushButton *btn2 = ask.addButton(QString("Choose: Level 3"), QMessageBox::AcceptRole);
    QPushButton *btn3 = ask.addButton(QString("Choose: Level 4"), QMessageBox::AcceptRole);
    ask.addButton(QString("Cancel"), QMessageBox::RejectRole);
    connect(btn0, SIGNAL(clicked()), SLOT(enableEPMode()));
    connect(btn1, SIGNAL(clicked()), SLOT(enablePMode()));
    connect(btn2, SIGNAL(clicked()), SLOT(enableSMode()));
    connect(btn3, SIGNAL(clicked()), SLOT(enableESMode()));
    ask.exec();
    delete btn0;
    delete btn1;
    delete btn2;
    delete btn3;
}


void browser::setMode(int input) //Sets mode to user selected mode.
{
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    settings.setValue("mode", input);
    *mode = input;
    switch(*mode) {
    case 0: //Extra Powerful

        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true); //Needed for 'plugins', ex. flash, to work
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, true); //Currently not supported ?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true); //Is javascript enabled?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true); //Does javascript have access to open windows? ex. Popups...
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true); //Does javascript have access to the user's clipboard? This may be seen as an invasion in privacy.
    break;
    case 1: //Powerful
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true); //Needed for 'plugins', ex. flash, to work
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, true); //Currently not supported ?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true); //Is javascript enabled?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false); //Does javascript have access to open windows? ex. Popups...
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false); //Does javascript have access to the user's clipboard? This may be seen as an invasion in privacy.
    break;
    case 2: //Secure
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true); //Needed for 'plugins', ex. flash, to work
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, false); //Currently not supported ?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, false); //Is javascript enabled?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false); //Does javascript have access to open windows? ex. Popups...
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false); //Does javascript have access to the user's clipboard? This may be seen as an invasion in privacy.
    break;
    case 3: //Extra Secure
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false); //Needed for 'plugins', ex. flash, to work
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, false); //Currently not supported ?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, false); //Is javascript enabled?
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false); //Does javascript have access to open windows? ex. Popups...
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false); //Does javascript have access to the user's clipboard? This may be seen as an invasion in privacy.
    break;
    }
}
void browser::enableEPMode() //Enable Extra Powerful Mode
{
    setMode(0);
    tabs->refreshAll();
}
void browser::enablePMode() //Enable Powerful Mode
{
    setMode(1);
    tabs->refreshAll();
}
void browser::enableSMode() //Enable Secure Mode
{
    setMode(2);
    tabs->refreshAll();
}
void browser::enableESMode() //Enable Extra Secure Mode
{
    setMode(3);
    tabs->refreshAll();
}

void browser::load_start() //Handles website loading start
{
    BrowserView *tmpWeb = qobject_cast<BrowserView*>(sender());
    if (!tmpWeb)
        return;

    tmpWeb->progress = 0;

    if (*noScriptEnabled)
        handleNoScript();

    if (whitelistON.contains(tmpWeb->url().host()))
        tmpWeb->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    else if (whitelistOFF.contains(tmpWeb->url().host()))
        tmpWeb->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);

    int tmpIdx = tabs->getTabIdx(tmpWeb);

    if (tmpIdx != -1) {
        if (tmpWeb->isLoading) {
            tmpWeb->stop();
        }
        tabs->setTabIcon(tmpIdx, QIcon(":/icons/loading.png"));
        tmpWeb->isLoading = true;
    }

    if (!(tabs->getCurrBrowser()))
        return;

    if (tmpWeb == tabs->getCurrBrowser()) {
        setStatusTxt("Beginning to load...");
        ui->webIcon->setIcon(QIcon(":/icons/loading.png"));
        canRefresh = false;
        ui->refreshBtn->setIcon(QIcon(":/icons/stop.png"));
        ui->refreshBtn->setToolTip("Stop");
    }
}

void browser::load_progress(int i) //Handles website loading progress [started but not finished]
{
    BrowserView *browser = qobject_cast<BrowserView*>(sender());
    if (!browser)
        return;
    if (i > 0 && i < 100)
        browser->progress = i;
    if (sender() == tabs->getCurrBrowser()) {
        QString str = "";
        str.setNum(i);
        setStatusTxt("Loading... " % str % "%");
    }
}

void browser::load_finish(bool b) //Handles website loading finish
{
    BrowserView *tmpWeb = qobject_cast<BrowserView*>(sender());
    if (!tmpWeb)
        return;

    if (b) {
        setStatusTxt("Done");
        tmpWeb->progress = 100;
    }
    else
        setStatusTxt("Done - With Errors");

    tmpWeb->isLoading = false;

    if (!b && *adBlock2Enabled() && tmpWeb->progress < 20) {
        QSettings settings("data/whitelist.dat", QSettings::IniFormat);
        if (settings.contains("adblock")) {
            if (!(settings.value("adblock").toStringList().contains(tmpWeb->url().host())))
                attemptBypassAdblock(tmpWeb->url());
        }
        else
            attemptBypassAdblock(tmpWeb->url());
    }

    if (!b)
        return;

    int tmpIdx = tabs->getTabIdx(tmpWeb);

    QIcon webIcon = tmpWeb->icon();
    if (webIcon.isNull())
        webIcon = QWebSettings::iconForUrl(tmpWeb->url());
    if (webIcon.isNull())
        webIcon = QIcon(":/icons/blank.png");

        tabs->setTabIcon(tmpIdx, webIcon);

    if (*adBlockEnabled()) {
        AdBlockPage *adBlockPg = new AdBlockPage();
        adBlockPg->applyRulesToPage(tmpWeb->page());
    }

    if (!(tabs->getCurrBrowser()))
        return;

    if (tmpWeb == tabs->getCurrBrowser()) {
        if (webIcon.isNull())
            ui->webIcon->setIcon(QIcon(":/icons/blank.png"));
        else
            ui->webIcon->setIcon(webIcon);
        canRefresh = true;
        ui->refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
        ui->refreshBtn->setToolTip("Refresh");
    }

    saveTabSession();

}

void browser::update_history(bool b) //Updates history. loadFinished signal calls this slot.
{
    Q_UNUSED(b);
    BrowserView *theBrowser = qobject_cast<BrowserView*>(sender());
    if (!theBrowser)
        return;
    QString url = theBrowser->url().toString();
    QString title = theBrowser->title();
    QDateTime lastVisit = QDateTime::currentDateTime();
    history->addItem(title, url, lastVisit);
}

void browser::showHistory() //Creates new QMainWindow and shows QTableView of all the history in there
{
    QMainWindow *tmp = new QMainWindow(this);
    tmp->setAttribute(Qt::WA_DeleteOnClose);
    tmp->setGeometry(geometry().x(), geometry().y() + 75, 600, 450);
    tmp->setMaximumWidth(600);
    tmp->resize(600, 450);
    tmp->setWindowTitle("History - Symphony");

    QTableView *historyView = new QTableView(tmp);
    historyList = new QStandardItemModel(tmp);
    historyList->insertColumn(0);
    historyList->insertColumn(1);
    historyList->insertColumn(2);
    historyList->setHeaderData(0, Qt::Horizontal, "Title", Qt::DisplayRole);
    historyList->setHeaderData(1, Qt::Horizontal, "Url", Qt::DisplayRole);
    historyList->setHeaderData(2, Qt::Horizontal, "Last Visit", Qt::DisplayRole);

    for (int i = 0; i < history->items().count(); i++) {
    historyList->insertRow(historyList->rowCount());
    historyList->setData(historyList->index(i, 0), history->items().at(i).title, Qt::DisplayRole);
    historyList->setData(historyList->index(i, 1), history->items().at(i).url, Qt::DisplayRole);
    historyList->setData(historyList->index(i, 2), history->items().at(i).lastVisited, Qt::DisplayRole);
    }

    historyView->setModel(historyList);
    historyView->setColumnWidth(0, 159);
    historyView->setColumnWidth(1, 300);
    historyView->setEditTriggers(NULL);

    connect(historyView, SIGNAL(doubleClicked(QModelIndex)), SLOT(click_history(QModelIndex)));

    tmp->setCentralWidget(historyView);
    tmp->show();
}

void browser::click_history(QModelIndex index) //Opens history link in new tab if the URL is clicked
{
    if (index.column() == 1) {
        addTab(index.data().toString(), false);
        raise();
    }
    else {
        addTab(historyList->itemData(historyList->index(index.row(), 1)).value(0).toString(), false);
        raise();
    }
}

void browser::changeAddressBar(QUrl url)
{
    if (!(tabs->getTab(tabs->count() - 1)))
        return;
    for(int i = 0; i < tabs->count(); i++) {
        if (!(tabs->tabBar->at(tabs->getTab(i))))
            continue;
        tabs->tabBar->at(tabs->getTab(i))->url = tabs->getTab(i)->getBrowser()->url().toString();
    }

    if (tabs->getCurrBrowser()->url() == url)
    if (ui->addressBar->text() != url.toString())
        ui->addressBar->setText(url.toString());
}

void browser::title_changed(QString theTitle) //Set the tab titles
{
    int maxChars = 26; //If title is longer than max chars, title is set to maxChars - 3 chars and "..." is added at the end
    QString theTitleFull = theTitle;
    QString modifiedTitle;

    int index = -1;

    for(int a = 0; a < tabs->count(); a++)
    {
        if (tabs->getTab(a)->getBrowser() == sender()) {
            index = a;
            if (tabs->getTab(a)->privateBrowsing ) {
                theTitleFull = "[Private] " % theTitleFull;
                modifiedTitle = theTitleFull;
                if (modifiedTitle.length() > maxChars) {
                    modifiedTitle.chop(modifiedTitle.length() - (maxChars - 3));
                    modifiedTitle += "...";
                }
            }
            else
            {
                modifiedTitle = theTitle;
                if (modifiedTitle.length() > maxChars) {
                    modifiedTitle.chop(modifiedTitle.length() - (maxChars - 3));
                    modifiedTitle += "...";
                }
            }
        }
      if (index != -1)
          break;
    }
    if (theTitleFull.isEmpty())
    {
        theTitleFull = "Loading...";
        modifiedTitle = "Loading...";
    }
    if (sender() == tabs->getCurrBrowser())
        setWindowTitle(theTitleFull % " - Symphony");

    //tabs->setTabText(index, modifiedTitle);
    tabs->setTabText(index, theTitleFull);
}

void browser::updateTabSession(int i)
{
    Q_UNUSED(i);
    saveTabSession();
}

void browser::update_status(QString txt) //If website has status to send, this will pick it up and display it
{
    if (txt.isEmpty())
        return;

    BrowserView *webView = qobject_cast<BrowserView*>(sender());
    if (!webView)
        return;

    BrowserView *currBrowser = tabs->getCurrBrowser();
    if (webView == currBrowser)
        setStatusTxt(txt);
}

void browser::handleLinkClicked(QUrl uri)
{
    if (uri.scheme() == QLatin1String("javascript"))
        tabs->getCurrBrowser()->page()->mainFrame()->evaluateJavaScript(QUrl::fromPercentEncoding(uri.toString(Q_FLAGS(QUrl::TolerantMode|QUrl::RemoveScheme)).toAscii()));
}

void browser::handleCloseWindow()
{
    if (tabs->count() > 1)
        tabs->removeTab(tabs->currentIndex());
    else
        doNavigate(*homePage); //Todo: Tell user that website is currently trying to close browser. Ask if they are okay with this. If no, navigate to homepage. If yes, close browser.
}

void browser::tab_changed(int index) //Changes data to what it should be whenever you switch tabs
{
    if (index == -1) {
        ui->addressBar->setText("blank");
        setWindowTitle("blank - Symphony");
        return;
    }
    if (tabs->currentIndex() == index)
        ui->addressBar->setText(tabs->getCurrBrowser()->url().toString());
    else if (ui->addressBar->text() != tabs->getTab(tabs->currentIndex())->getBrowser()->url().toString())
        ui->addressBar->setText(tabs->getCurrBrowser()->url().toString());

    QIcon webIcon = tabs->getTab(tabs->currentIndex())->getBrowser()->icon();
    if (webIcon.isNull())
        webIcon = QIcon(":/icons/blank.png");
    ui->webIcon->setIcon(webIcon);

    QString webTitle(tabs->getCurrBrowser()->title());
    if (!webTitle.isEmpty())
        setWindowTitle(webTitle % " - Symphony");
    else
        setWindowTitle("blank - Symphony");

    if (!(tabs->getTab(index)->getBrowser()->isLoading))
        setStatusTxt("Done");
    else
        setStatusTxt("Loading...");

    if (tabs->getTab(index)->getBrowser()->isLoading) {
        canRefresh = false;
        ui->refreshBtn->setIcon(QIcon(":/icons/stop.png"));
        ui->refreshBtn->setToolTip("Stop");
    }
    else {
        canRefresh = true;
        ui->refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
        ui->refreshBtn->setToolTip("Refresh");
    }
    if (*noScriptEnabled)
        handleNoScript();
}

void browser::link_hovered(QString url, QString title, QString text) //Handles what happens when a link is hovered over
{
    Q_UNUSED(text);
    Q_UNUSED(title);
    if (url.isEmpty()) {
        setStatusTxt("");
        return;
    }
    setStatusTxt(url);
}

void browser::showFind() //Shows find dialog
{
    findWind->show();
}
void browser::doFind() //Occurs on using find dialog.
{
    bool caseSens = findDialog->caseSensitiveBox->isChecked();
    bool findReverse = findDialog->findBackwardsBox->isChecked();
    bool highlight = findDialog->highlightAllBox->isChecked();
    bool wrapAround = findDialog->wrapPageBox->isChecked();
    QWebPage::FindFlags options;
    if (caseSens)
        options |= QWebPage::FindCaseSensitively;
    if (findReverse)
        options |= QWebPage::FindBackward;
    if (highlight)
        options |= QWebPage::HighlightAllOccurrences;
    if (wrapAround)
        options |= QWebPage::FindWrapsAroundDocument;

    tabs->getCurrBrowser()->findText(findDialog->findInput->text(), options);
}

void browser::closeFind() //Closes find dialog and resets everything.
{
    findDialog->findInput->setText("");
    findDialog->caseSensitiveBox->setChecked(false);
    findDialog->findBackwardsBox->setChecked(false);
    findDialog->highlightAllBox->setChecked(false);
    findDialog->wrapPageBox->setChecked(false);
    findWind->close();
}

void browser::saveAs() //Save website as HTML file
{

    QString fileName = QFileDialog::getSaveFileName( this
    ,tr("Save Web Site")
    ,QDir::currentPath()
    ,tr("HTML (*.html)") );
    BrowserView *currBrowser = tabs->getCurrBrowser();
    QString fileHtml;
    if (currBrowser != NULL)
        fileHtml = currBrowser->page()->mainFrame()->toHtml();
    else
        fileHtml = "";

    QByteArray tmp;
    tmp.append(fileHtml);
    QFile *f = new QFile(fileName);
    f->open(QIODevice::WriteOnly);
    f->write(tmp);
    f->close();
    delete f;
}

void browser::doPrint() //Print website
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer);
    if (dialog->exec() == QDialog::Accepted) {
        BrowserView *currBrowser = tabs->getCurrBrowser();
        currBrowser->print(&printer);
    }
    delete dialog;
}

void browser::enableInspect() //Toggles inspect [web dev]
{
    if (!inspectEnabled) {
        for(int a = 0; a < tabs->count(); a++) {
            BrowserView *tmpBrowser = tabs->getTab(a)->getBrowser();
            tmpBrowser->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled , true);
        }
        inspectEnabled = true;
        siteInspection->setChecked(true);
    }
    else {
        for(int b = 0; b < tabs->count(); b++) {
                BrowserView *tmpBrowser = tabs->getTab(b)->getBrowser();
                tmpBrowser->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled , false);
        }
        inspectEnabled = false;
        siteInspection->setChecked(false);
    }
}
void browser::enableModification() //Toggles modification [web dev]
{

    if (!modificationEnabled) {
        for(int a = 0; a < tabs->count(); a++) {
            BrowserView *tmpBrowser = tabs->getTab(a)->getBrowser();
            tmpBrowser->page()->setContentEditable(true);
        }
        modificationEnabled = true;
        contentMod->setChecked(true);
   }
    else {
        for(int b = 0; b < tabs->count(); b++) {
            BrowserView *tmpBrowser = tabs->getTab(b)->getBrowser();
            tmpBrowser->page()->setContentEditable(false);
        }
        modificationEnabled = false;
        contentMod->setChecked(false);
    }
}

void browser::showSource() //Show current web site source
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    QString fileHtml;

    fileHtml = currBrowser->page()->mainFrame()->toHtml();
    QMainWindow *tmp = new QMainWindow(this);
    tmp->setGeometry(geometry().x(), geometry().y(), 550, 650);
    tmp->resize(550, 650);
    tmp->setWindowTitle("View Source - Symphony");
    QTextEdit *txtEditor = new QTextEdit();
    Highlighter *htmlSyntaxer = new Highlighter(txtEditor->document());
    txtEditor->setGeometry(tmp->geometry());
    txtEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tmp->setCentralWidget(txtEditor);
    tmp->show();
    txtEditor->setPlainText(fileHtml);

    Q_UNUSED(htmlSyntaxer);
}

void browser::incTextSize() //Increases text size
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    if (currBrowser->zoomFactor() < 2.5)
    currBrowser->setZoomFactor(currBrowser->zoomFactor() + 0.1);
}
void browser::decTextSize() //Decreases text size
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    if (currBrowser->zoomFactor() > 0.25)
    currBrowser->setZoomFactor(currBrowser->zoomFactor() - 0.1);
}
void browser::resetTextSize() //Sets text size back to normal
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    currBrowser->setZoomFactor(1);
}

void browser::showDownloads() //Shows download manager
{
    alert("Download manager is under construction!");
}

void browser::addPrivateTab() //Adds private tab
{
    addTab(*homePage, true);
}

void browser::togglePrivateBrowsing() //Toggles private browsing
{
    if (!globalPrivateBrowsing) {
        saveTabs();
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
        addTab(*homePage, true);
        privBrowsing->setChecked(true);
    }
    else {
        QWebSettings::globalSettings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, false);
        restoreTabs();
        privBrowsing->setChecked(false);
    }
}

void browser::loadBookmark(const QString &url) //Opens bookmarked URL
{
    addTab(url, false);
}

void browser::finishNavigating(QHostInfo host) //If website the user is attempting to go to is valid, then go to it. Otherwise, google search their invalid URL
{
    if (host.hostName().isEmpty() || !tabs->getCurrBrowser())
        return;
    setStatusTxt("Finished resolving.");
    if (host.error() == 0) //If -no- errors
        tabs->getCurrBrowser()->setUrl(tabs->getCurrBrowser()->initialUrl());
    else //If errors, google search the attempted URL
        tabs->getCurrBrowser()->setUrl(QUrl(QString("http://www.google.com/search?q=") % ui->addressBar->text())); //TODO: More search engine variety);
}

void browser::showPreferences() //Shows preferences window
{
    prefWind->exec();
}

void browser::showShortcuts() //Shows QMessageBox with more info about shortcuts
{
    alert("Address Bar Shortcuts:\n\n"
          "Enter - Go to specified URL\n"
          "Up/Down Arrows - Navigate through history URLs\n"
          "Ctrl+Enter - Add .com to the end of your entered text and go\n"
          "Shift+Enter - Add .net to the end of your entered text and go\n"
          "Shift+Ctrl+Enter - Add .org to the end of your entered text and go"
          "\n\n\n"
          "Normal Shortcuts:\n\n"
          "Ctrl+Left/Right Arrows - Switch to the next/previous tab\n"
          "Ctrl+[+] - Increase text size\n"
          "Ctrl+[-] - Decrease text size\n"
          "Ctrl+[0] - Reset text size\n"
          "Ctrl+[A] - Select all text\n"
          "Ctrl+[F] - Show find dialog\n"
          "Ctrl+[N] - New window\n"
          "Ctrl+[P] - Print current page\n"
          "Ctrl+[Q] - Focus on address bar\n"
          "Ctrl+[S] - Save current page to disk\n"
          "Ctrl+[T] - New tab\n"
          "Ctrl+Shift+[T] - Restore last closed tab"
          "\n\n\n"
          "Mouse gestures:\n"
          "To do a mouse gesture, hold the right click button on your mouse and drag your mouse in the desired direction; then let go of the right click button.\n\n"
          "Down - Reload page\n"
          "Up - Reload page and bypass cache\n"
          "Left - Back\n"
          "Right - Forward");
}

void browser::showAbout() //The about window of course - showed in a QMessageBox
{
    alert("Symphony\nVersion: Alpha V0.0.006\nLicensed under the GNU General Public License V3.0\nCopyright  2011-Now  SymphSoftware  All Rights Reserved");
}

void browser::on_homeBtn_clicked() //Go 'home'
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    currBrowser->setUrl(QUrl(*homePage));
}

void browser::on_fwdBtn_clicked() //Go 'forward'
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    currBrowser->forward();
}

void browser::on_backBtn_clicked() //Go 'back'
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    currBrowser->back();
}

void browser::on_refreshBtn_clicked() //Refresh or stop, depending on what is currently possible
{
    if (canRefresh) {
        tabs->getCurrBrowser()->reload();
        canRefresh = false;
        ui->refreshBtn->setIcon(QIcon(":/icons/stop.png"));
        ui->refreshBtn->setToolTip("Stop");
    }
    else {
        tabs->getCurrBrowser()->stop();
        canRefresh = true;
        ui->refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
        ui->refreshBtn->setToolTip("Refresh");
    }
}

void browser::on_mainBtn_clicked() //Show main menu button options
{
    QMenu *subMenu = new QMenu(this);
    subMenu->setTitle("Web Developer Features");
    subMenu->addAction(siteInspection);
    subMenu->addAction(contentMod);

    QMenu *menu = new QMenu(this);
    menu->addAction("Set Browser Security Mode", this, SLOT(setMode()));
    menu->addAction("New Private Tab", this, SLOT(addPrivateTab()));
    menu->addSeparator();
    menu->addAction("Find", this, SLOT(showFind()), Qt::CTRL + Qt::Key_F);
    menu->addAction("Save As", this, SLOT(saveAs()), Qt::CTRL + Qt::Key_S);
    menu->addAction("Print Page", this, SLOT(doPrint()), Qt::CTRL + Qt::Key_P);
    menu->addSeparator();
    menu->addAction("Downloads", this, SLOT(showDownloads()));
    menu->addAction("History", this, SLOT(showHistory()), Qt::CTRL + Qt::Key_H);
    menu->addMenu(subMenu);

    menu->addAction(privBrowsing);
    menu->addAction("Preferences", this, SLOT(showPreferences()));
    menu->addSeparator();
    menu->addAction("Shortcuts", this, SLOT(showShortcuts()));
    menu->addAction("About", this, SLOT(showAbout()));
    menu->popup(QPoint(QCursor::pos().x() + 10, QCursor::pos().y() - 10));

}

void browser::on_statusBtn_clicked() //Occurs when status button in bottom left is clicked
{
    QMenu *menu = new QMenu(this);
    menu->addAction("View Source", this, SLOT(showSource()));
    menu->addAction("Toggle Popups For This Site", this, SLOT(togglePopupPerm()));
    if (*adBlock2Enabled() || *adBlockEnabled())
        menu->addAction("Disable AdBlock For This Site", this, SLOT(disableAdBlockSite()));
    menu->addSeparator();
    menu->addAction("Zoom In", this, SLOT(incTextSize()), Qt::CTRL + Qt::Key_Plus);
    menu->addAction("Zoom Out", this, SLOT(decTextSize()), Qt::CTRL + Qt::Key_Minus);
    menu->addAction("Reset Zoom", this, SLOT(resetTextSize()), Qt::CTRL + Qt::Key_0);
    menu->popup(QPoint(QCursor::pos().x() + 10, QCursor::pos().y() - 10));
}

void browser::on_bookmarkBtn_clicked() //Displays all current bookmarks in QMenu
{
    QMenu *menu = new QMenu(this);
    QSignalMapper *signalMapper = new QSignalMapper(this);
    if (bookmarks->items().count() > 0) {
        for (int i = 0; i < bookmarks->items().count(); i++) {
            QAction *tmpAction = menu->addAction(bookmarks->items().at(i).title, signalMapper, SLOT(map()));
            signalMapper->setMapping(tmpAction, bookmarks->items().at(i).url);
        }
    }
    else
        menu->addAction("None");

    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(loadBookmark(const QString &)));
    menu->popup(QPoint(QCursor::pos().x() + 10, QCursor::pos().y() + 10));

}

void browser::on_addBookmark_clicked() //Adds current site to bookmarks
{
    BrowserView *currBrowser = tabs->getCurrBrowser();
    addBookmark(currBrowser->url().toString(), currBrowser->title());
}

void browser::on_goBtn_clicked() //Navigate when Go button is clicked...
{
    doNavigate();
}

void browser::on_addTabBtn_clicked() //Open tab
{
    if (shiftDown) {
        BrowserView *currBrowser = tabs->getCurrBrowser();
        addTab(currBrowser->url().toString(), false);
    }
    else
        addTab(*homePage, false);
}

void browser::on_noScriptBtn_clicked()
{
    refreshNoScriptList();
    noScriptWind->show();
}

void browser::keyPressEvent(QKeyEvent *event) //On key p ress. Handles all the shortcuts.
{
    /* //Display which integer is being pressed via message box
    QString *tmp = new QString();
    tmp->setNum(event->key());
    alert(*tmp); */

     switch(event->key()) {
     case Qt::Key_Escape:
         if (tabBar->tabMenu->isVisible())
             tabBar->tabMenu->hide();
         break;
     case 16777220: //Enter
     case 16777221: //Num-Pad Enter
        if (ui->addressBar->hasFocus())
        {
            if (ctrlDown && shiftDown)
                ui->addressBar->setText(ui->addressBar->text() % ".org");
            else if (ctrlDown)
                ui->addressBar->setText(ui->addressBar->text() % ".com");
            else if (shiftDown)
                ui->addressBar->setText(ui->addressBar->text() % ".net");
            doNavigate();
        }
         break;
     case 16777234: //Left Arrow
         if (ctrlDown) {
            tabs->setFocus();
            if (tabs->currentIndex() == 0)
               tabs->setCurrentIndex(tabs->count() - 1);
            else
               tabs->setCurrentIndex(tabs->currentIndex() - 1);
         }
     break;
     case 16777236: //Right Arrow
         if (ctrlDown) {
             tabs->setFocus();
             if (tabs->currentIndex() == tabs->count() - 1)
                tabs->setCurrentIndex(0);
             else
                tabs->setCurrentIndex(tabs->currentIndex() + 1);
         }
     break;
     case 16777249: //CTRL
     ctrlDown = true;
         break;
     case 16777248: //Shift
     shiftDown = true;
         break;
     case 96: //~
         break;
     case 43: //+ Button
     case 61: //= & + Button
         incTextSize();
         break;
     case 45: //- Button
         decTextSize();
         break;
     case 48: //0
         resetTextSize();
         break;
     case 49: //1
         break;
     case 50: //2
         break;
     case 51: //3
         break;
     case 52: //4
         break;
     case 53: //5
         break;
     case 54: //6
         break;
     case 55: //7
         break;
     case 56: //8
         break;
     case 57: //9
         break;
     case 65: //A
         if (ctrlDown)
             tabs->getCurrBrowser()->page()->triggerAction(QWebPage::SelectAll);
         break;
     case 66: //B
         break;
     case 67: //C
         break;
     case 68: //D
         break;
     case 69: //E
         break;
     case 70: //F
         showFind();
         break;
     case 71: //G
         break;
     case 72: //H
         showHistory();
         break;
     case 73: //I
         break;
     case 74: //J
         break;
     case 75: //K
         break;
     case 76: //L
         break;
     case 77: //M
         break;
     case 78: //N
         if (ctrlDown)
            newWindow();
         break;
     case 79: //O
         if (ctrlDown)
             ui->addressBar->setFocus();
         break;
     case 80: //P
         if (ctrlDown)
            doPrint();
         break;
     case 81: //Q
        if(ctrlDown)
            ui->addressBar->setFocus();
         break;
     case 82: //R
         break;
     case 83: //S
         if (ctrlDown)
             saveAs();
         break;
     case 84: //T
        if (ctrlDown) {
            if (shiftDown)
               tabs->restoreTab();
            else
                addTab(*homePage, false);
        }
         break;
     case 85: //U
         break;
     case 86: //V
         break;
     case 87: //W
         break;
     case 88: //X
         break;
     case 89: //Y
         break;
     case 90: //Z
         break;
     }
     QMainWindow::keyPressEvent(event);
}

void browser::keyReleaseEvent(QKeyEvent *event) //On key release. Handles key modifiers mainly [i.e. CTRL & Shift]
{
    switch(event->key()) {
    case Qt::Key_Control: //CTRL
        ctrlDown = false;
        break;
    case Qt::Key_Shift: //Shift
        shiftDown = false;
        break;
    }

    QMainWindow::keyReleaseEvent(event);
}

void browser::moveEvent(QMoveEvent *e) //Overrides moveEvent to emit moved signal
{
    QMainWindow::moveEvent(e);
    emit moved(e->pos());
}

void browser::resizeEvent(QResizeEvent* e) //Resize the tabs and such to fit correctly
{
    int topFiller = ui->topBar->height();
    int bottomFiller = ui->statusBar->height(); //Put extra height accumulated from all controls below this one
    tabBar->resize(geometry().width(), 30);
    ui->topBar->resize(geometry().width(), ui->topBar->height());
    tabs->tabBar->refreshTabScrollers();
    tabs->resize(QSize(geometry().width(), geometry().height() - bottomFiller - topFiller - 30));

    ui->statusBar->setGeometry(0, geometry().height() - ui->statusBar->height(), geometry().width() - 2, ui->statusBar->height());
    ui->statusBtn->move(ui->statusBar->x(), ui->statusBar->y() + 1);

    QMainWindow::resizeEvent(e);
}
