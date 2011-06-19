#ifndef LOADER_H
#define LOADER_H
#include <QObject>
#include "history.h"
#include "bookmarks.h"
#include "adblocksubscription.h"
#include "downloadmanager.h"
#include <QNetworkDiskCache>
#include "browserview.h"
#include "cookiejar.h"
#include "networkaccessmanager.h"
#include "noscript.h"

class CookieJar;
class History;
class Bookmarks;
class AdBlockSubscription;
class DownloadManager;
class BrowserView;
class NetworkAccessManager;
class NoScript;
class Loader : public QObject
{
    Q_OBJECT

public:
    Loader(QObject *parent = 0);
    ~Loader();
    void newBrowser(bool preloadTabs, BrowserView *view = 0);
    CookieJar *cookieJar;
    History *history;
    Bookmarks *bookmarks;
    NoScript *noScript;
    AdBlockSubscription *subscrip;
    NetworkAccessManager *accessMgr;
    QNetworkDiskCache *cache;
    DownloadManager *downMgr;
    QStringList *sslWhiteList;
    QString *homePage;
    bool *adBlockEnabled;
    bool *adBlock2Enabled;
    bool *noScriptEnabled;
    bool *noScriptSecureEnabled;
    int *mode;

private:
    int m_mode;
    int browsersOpen;
    bool m_adBlockEnabled;
    bool m_adBlock2Enabled;
    bool m_noScriptEnabled;
    bool m_noScriptSecureEnabled;

private slots:
    void checkClose();
    void finishLoad();
    void autoSave();

};

#endif // LOADER_H
