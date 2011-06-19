#ifndef BROWSER_H
#define BROWSER_H

#include <QMainWindow>
#include <QUrl>
#include <QWebView>
#include <QSettings>
#include <QTabBar>
#include <QModelIndex>
#include <QtNetwork/QNetworkRequest>
#include <QSslError>
#include <QStringList>
#include <QHostInfo>
#include "completer.h"
#include "tabs.h"
#include "tabbar.h"
#include "browser.h"
#include "highlighter.h"
#include "tab.h"
#include "history.h"
#include "completer.h"
#include "bookmarks.h"
#include "downloadmanager.h"
#include "preferences.h"
#include "networkaccessmanager.h"
#include "adblocksubscription.h"
#include "loader.h"
#include "cookiejar.h"
#include "noscript.h"

namespace Ui {
    class browser;
    class findDialog;
}

class AdBlockSubscription;
class NetworkAccessManager;
class Tabs;
class TabBar;
class SymphPluginFactory;
class BrowserView;
class Highlighter;
class TabBar;
class Tab;
class History;
class Completer;
class Bookmarks;
class DownloadManager;
class Preferences;
class Loader;
class CookieJar;
class NoScript;

class browser : public QMainWindow
{
    Q_OBJECT

public:
    explicit browser(QWidget *parent = 0, Loader *v_loader = 0);
    ~browser();
    void doLoad();
    void attemptBypassAdblock(QUrl badUrl);
    void setSubscrip(AdBlockSubscription *sub);
    void setAdBlockEnabled(bool *b);
    void setAdBlock2Enabled(bool *b);
    void setNoScriptEnabled(bool *b);
    void handleNoScript();
    void updateNoScriptSecure();
    void loadNoScriptWind();
    void updateWhitelist();
    void addTab(QString theUrl, bool privateBrowsing);
    void addTab(QString theTitle, bool privateBrowsing, BrowserView *newWeb);
    void saveTabs();
    void restoreTabs();
    void setStatusTxt(QString txt);
    void addBookmark(QString url, QString title);
    void doNavigate(QString = QString());
    void alert(QString str);
    void saveTabSession();
    void newWindow();
    static AdBlockSubscription *subscrip();
    static bool* adBlockEnabled();
    static bool* adBlock2Enabled();
    QMainWindow *noScriptWind;
    QDialog *findWind;
    TabBar *tabBar;
    Loader *loader;
    QString *homePage;
    Tabs *tabs;
    CookieJar *cookieJar;
    History *history;
    Bookmarks *bookmarks;
    Completer *completer;
    QStringList publicTabHistory;
    Preferences *prefWind;
    DownloadManager *downMgr;
    QTableView *noScriptView;
    NoScript *noScript;
    BrowserView *initView;
    NetworkAccessManager *accessMgr;
    QStringList *sslWhiteList;
    QStandardItemModel *historyList;
    QStandardItemModel *noScriptList;
    QAction *privBrowsing;
    QAction *siteInspection;
    QAction *contentMod;
    bool *noScriptEnabled;
    bool *noScriptSecureEnabled;
    int *mode;
    QStringList whitelistON;
    QStringList whitelistOFF;
    bool canRefresh;
    bool ctrlDown;
    bool shiftDown;
    bool inspectEnabled;
    bool globalPrivateBrowsing;
    bool modificationEnabled;
    bool doPreloadTabs;

private:
    Ui::browser *ui;
    Ui::findDialog *findDialog;
    static AdBlockSubscription *m_subscrip;
    static bool *m_adBlockEnabled;
    static bool *m_adBlock2Enabled;
    bool m_canLoad;
    bool m_newWindow;

private slots:
    void disableAdBlockSite();
    void refreshNoScriptList();
    void click_noScriptWind(QModelIndex index);
    void togglePopupPerm();
    void handleSSL(QNetworkReply*,QList<QSslError>);
    void setMode();
    void setMode(int input);
    void enableEPMode();
    void enablePMode();
    void enableSMode();
    void enableESMode();
    void load_start();
    void load_progress(int i);
    void load_finish(bool b);
    void update_history(bool b);
    void showHistory();
    void click_history(QModelIndex index);
    void changeAddressBar(QUrl url);
    void title_changed(QString str);
    void updateTabSession(int i);
    void update_status(QString txt);
    void handleLinkClicked(QUrl uri);
    void handleCloseWindow();
    void tab_changed(int index);
    void link_hovered(QString url, QString title, QString text);
    void showFind();
    void doFind();
    void closeFind();
    void saveAs();
    void doPrint();
    void enableInspect();
    void enableModification();
    void showSource();
    void incTextSize();
    void decTextSize();
    void resetTextSize();
    void showDownloads();
    void addPrivateTab();
    void togglePrivateBrowsing();
    void loadBookmark(const QString &);
    void finishNavigating(QHostInfo);
    void showPreferences();
    void showShortcuts();
    void showAbout();
    void on_homeBtn_clicked();
    void on_fwdBtn_clicked();
    void on_backBtn_clicked();
    void on_refreshBtn_clicked();
    void on_mainBtn_clicked();
    void on_statusBtn_clicked();
    void on_bookmarkBtn_clicked();
    void on_addBookmark_clicked();
    void on_goBtn_clicked();
    void on_addTabBtn_clicked();
    void on_noScriptBtn_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *event);

signals:
    void moved(const QPoint e);
};

#endif // BROWSER_H
