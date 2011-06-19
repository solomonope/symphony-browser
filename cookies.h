#ifndef COOKIES_H
#define COOKIES_H

#include <QDateTime>
#include <QList>
#include <QtXml/QDomElement>
#include "networkcookieitem.h"

class NetworkCookieItem;
class Cookies
{

public:
    Cookies();
    void loadCookies();
    void insertItem(NetworkCookieItem tmp);
    void tryRemove(QString url);
    void doRemoveAt(int i);
    void clearAll();
    void saveCookiesFile();
    QList<NetworkCookieItem> items() const;
    QDomElement NetworkCookieItemToNode( QDomDocument &d, const NetworkCookieItem &h );

private:
    QList<NetworkCookieItem> m_items;

};

#endif // COOKIESS_H
