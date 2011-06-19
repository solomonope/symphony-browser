#include "loader.h"
#include "browser.h"
#include "qdebug.h"

Loader::Loader(QObject *parent)
    :QObject(parent)
{
    browsersOpen = 0;

    QSettings settings("data/settings.dat", QSettings::IniFormat);

    history = new History();

    cookieJar = new CookieJar();

    cookieJar->doLoadCookies();

    bookmarks = new Bookmarks();

    if (!settings.contains("mode"))
        settings.setValue("mode", 1);

    m_mode = settings.value("mode").toInt();
    mode = &m_mode;


    m_adBlockEnabled = false;
    m_adBlock2Enabled = false;
    m_noScriptEnabled = false;
    m_noScriptSecureEnabled = false;
    adBlockEnabled = &m_adBlockEnabled;
    adBlock2Enabled = &m_adBlock2Enabled;
    noScriptEnabled = &m_noScriptEnabled;
    noScriptSecureEnabled = &m_noScriptSecureEnabled;

    if (settings.contains("AdBlockEnabled")) {
        if (settings.value("AdBlockEnabled").toInt() == 1) {
            m_adBlockEnabled = true;
            adBlockEnabled = &m_adBlockEnabled;
        }
    }
    if (settings.contains("NoScriptEnabled")) {
        if (settings.value("NoScriptEnabled").toInt() == 1) {
            m_noScriptEnabled = true;
            noScriptEnabled = &m_noScriptEnabled;
        }
    }
    if (settings.contains("NoScriptSecureEnabled")) {
        if (settings.value("NoScriptSecureEnabled").toInt() == 1) {
            m_noScriptSecureEnabled = true;
            noScriptSecureEnabled = &m_noScriptSecureEnabled;
        }
    }
    if (settings.contains("AdBlock2Enabled")) {
        if (settings.value("AdBlock2Enabled").toInt() == 1) {
            m_adBlock2Enabled = true;
            adBlock2Enabled = &m_adBlock2Enabled;
        }
    }

    noScript = new NoScript();
    noScript->defaultSecure = noScriptSecureEnabled;

    QSettings settingsWL("data/whitelist.dat", QSettings::IniFormat);
    if (settingsWL.contains("ssl"))
        sslWhiteList = new QStringList(settingsWL.value("ssl").toStringList());
    else
        sslWhiteList = new QStringList();

    homePage = new QString("http://google.com");

    if (settings.contains("HomePage"))
        homePage = new QString(settings.value("HomePage").toString());
    downMgr = new DownloadManager(this);

    QFile webIcons("data/WebpageIcons.db");
    if (webIcons.exists())
        webIcons.remove(); //Not removing this causes possible periodic glitches with website favicons.

    cache = new QNetworkDiskCache(this);
    cache->setCacheDirectory("data/cache");

    subscrip = new AdBlockSubscription(this, QUrl("https://easylist-downloads.adblockplus.org/easylist.txt")); //At some point, this needs to be heavily optimized. It currently downloads a new easylist each time this application loads. This is the main thing that is heavily slowing down the loading time of Symphony.
    connect(subscrip, SIGNAL(loadRulesFinished()), SLOT(finishLoad()));

    accessMgr = new NetworkAccessManager(this, cookieJar, cache);

    subscrip->load();
}

Loader::~Loader()
{
    history->saveHistoryFile();
    delete history;
    bookmarks->saveBookmarksFile();
    delete bookmarks;
    cookieJar->saveCookies();
    delete cookieJar;
    noScript->saveAll();
    delete noScript;
    delete homePage;
}

void Loader::newBrowser(bool preloadTabs, BrowserView *view)
{
    browsersOpen++;
    if (view) {
        browser *wind = new browser(0, this);
        connect(wind, SIGNAL(destroyed()), SLOT(checkClose()));
        wind->doPreloadTabs = preloadTabs;
        wind->mode = mode;
        wind->setAdBlockEnabled(adBlockEnabled);
        wind->setAdBlock2Enabled(adBlock2Enabled);
        wind->homePage = homePage;
        wind->history = history;
        wind->bookmarks = bookmarks;
        wind->downMgr = downMgr;
        wind->setSubscrip(subscrip);
        wind->cookieJar = cookieJar;
        wind->accessMgr = accessMgr;
        wind->sslWhiteList = sslWhiteList;
        wind->noScriptEnabled = noScriptEnabled;
        wind->noScriptSecureEnabled = noScriptSecureEnabled;
        wind->noScript = noScript;
        wind->initView = view;
        wind->doLoad();
    }
    else {
        browser *wind = new browser(0, this);
        connect(wind, SIGNAL(destroyed()), SLOT(checkClose()));
        wind->doPreloadTabs = preloadTabs;
        wind->mode = mode;
        wind->setAdBlockEnabled(adBlockEnabled);
        wind->setAdBlock2Enabled(adBlock2Enabled);
        wind->homePage = homePage;
        wind->history = history;
        wind->bookmarks = bookmarks;
        wind->downMgr = downMgr;
        wind->setSubscrip(subscrip);
        wind->cookieJar = cookieJar;
        wind->accessMgr = accessMgr;
        wind->sslWhiteList = sslWhiteList;
        wind->noScriptEnabled = noScriptEnabled;
        wind->noScriptSecureEnabled = noScriptSecureEnabled;
        wind->noScript = noScript;
        wind->doLoad();
    }
}

void Loader::checkClose()
{
    browsersOpen--;
    if (browsersOpen == 0)
        deleteLater();
}

void Loader::finishLoad()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(autoSave()));
    timer->start(300000); //Every 5 minutes
    newBrowser(true);
}

void Loader::autoSave()
{
    cookieJar->saveCookies();
    history->saveHistoryFile();
    bookmarks->saveBookmarksFile();
    noScript->saveAll();
}
