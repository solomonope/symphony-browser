#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QNetworkCookieJar>
#include "cookies.h"

class Cookies;

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT

public:
    CookieJar(QObject *parent = 0);
    ~CookieJar();
    void doLoadCookies();
    void purgeCookies();
    void saveCookies();
    QList<QNetworkCookie> cookiesForUrl(QUrl &url);
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    Cookies *cookies;
    QList<QNetworkCookie> savedCookies;
};

#endif // COOKIEJAR_H
