#ifndef NETWORKCOOKIEITEM_H
#define NETWORKCOOKIEITEM_H

#include <QString>
#include <QByteArray>
#include <qDateTime>
#include <QDomElement>
#include <QDomDocument>

class NetworkCookieItem
{

public:
    NetworkCookieItem();
    NetworkCookieItem(QDomElement &e);
    NetworkCookieItem(QString m_domain, QDateTime m_expirationDate, QByteArray m_name, QString m_path, QByteArray m_value, bool m_httpOnly, bool m_secure);
    QDomElement createXMLNode(QDomDocument &d);
    QString domain;
    QDateTime expirationDate;
    QByteArray name;
    QString path;
    QByteArray value;
    bool httpOnly;
    bool secure;
};

#endif // NETWORKCOOKIEITEM_H
