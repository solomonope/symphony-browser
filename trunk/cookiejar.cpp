#include "cookiejar.h"
#include <QSettings>
#include <QMetaType>
#include <QDateTime>
#include "cookies.h"
#include <QWebSettings>
#include <qdebug.h>
CookieJar::CookieJar(QObject *parent)
    :QNetworkCookieJar(parent)
{}

CookieJar::~CookieJar()
{
    delete cookies;
}

void CookieJar::doLoadCookies()
{
    cookies = new Cookies();
    cookies->loadCookies();
    QList<NetworkCookieItem> tmp = cookies->items();
    QList<QNetworkCookie> cookieList;

    if (tmp.count() > 0) {
        for(int i = 0; i < tmp.count(); i++) {
            NetworkCookieItem nci = tmp.at(i);
            QNetworkCookie tmpCookie;
            tmpCookie.setDomain(nci.domain);
            tmpCookie.setExpirationDate(nci.expirationDate);
            tmpCookie.setHttpOnly(nci.httpOnly);
            tmpCookie.setName(nci.name);
            tmpCookie.setPath(nci.path);
            tmpCookie.setSecure(nci.secure);
            tmpCookie.setValue(nci.value);
            cookieList.insert(cookieList.count(), tmpCookie);
        }
        setAllCookies(cookieList);
        purgeCookies();
    }
}

void CookieJar::purgeCookies()
{
    QList<QNetworkCookie> cookieList = allCookies();
    foreach(QNetworkCookie cookie, cookieList)
    {
        QDateTime now = QDateTime::currentDateTime();
        if (cookie.expirationDate().isValid()) {
            if (cookie.expirationDate() < now)
                cookieList.removeOne(cookie);
        }
    }
    setAllCookies(cookieList);
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(QUrl &url)
{
    if (QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return QList<QNetworkCookie>();
    return QNetworkCookieJar::cookiesForUrl(url);
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    purgeCookies();

    return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

void CookieJar::saveCookies()
{
    cookies->clearAll();
    foreach(QNetworkCookie cookie, allCookies())
        cookies->insertItem(NetworkCookieItem(cookie.domain(), cookie.expirationDate(), cookie.name(), cookie.path(), cookie.value(), cookie.isHttpOnly(), cookie.isSecure()));
    cookies->saveCookiesFile();
}
