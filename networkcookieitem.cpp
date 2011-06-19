#include "networkcookieitem.h"

NetworkCookieItem::NetworkCookieItem()
{}

NetworkCookieItem::NetworkCookieItem(QDomElement &e)
{
    domain = e.attribute("domain", "");
    expirationDate = QDateTime::fromString(e.attribute("expirationDate", ""));
    name = e.attribute("name", "").toAscii();
    path = e.attribute("path", "");
    value = e.attribute("value", "").toAscii();
    int iHttpOnly = e.attribute("httpOnly", "").toInt();
    int iSecure = e.attribute("secure", "").toInt();
    if (iHttpOnly == 0)
        httpOnly = false;
    else
        httpOnly = true;
    if (iSecure == 0)
        secure = false;
    else
        secure = true;
}

NetworkCookieItem::NetworkCookieItem(QString m_domain, QDateTime m_expirationDate, QByteArray m_name, QString m_path, QByteArray m_value, bool m_httpOnly, bool m_secure) //Constructor 2
{
    domain = m_domain;
    expirationDate = m_expirationDate;
    name = m_name;
    path = m_path;
    value = m_value;
    httpOnly = m_httpOnly;
    secure = m_secure;

}

QDomElement NetworkCookieItem::createXMLNode(QDomDocument &d) //Turns QDomDocument in to QDomElement
{
    QDomElement nci = d.createElement( "NetworkCookieItem" );
    nci.setAttribute("domain", domain);
    nci.setAttribute("expirationDate", expirationDate.toString());
    nci.setAttribute("name", QString(name));
    nci.setAttribute("path", path);
    nci.setAttribute("value", QString(value));
    if (httpOnly)
        nci.setAttribute("httpOnly", 1);
    else
        nci.setAttribute("httpOnly", 0);
    if (secure)
        nci.setAttribute("secure", 1);
    else
        nci.setAttribute("secure", 0);
    return nci;
}
