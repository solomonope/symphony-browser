#ifndef BOOKMARKSITEM_H
#define BOOKMARKSITEM_H

#include <QString>
#include <QDomElement>
#include <QDomDocument>

class BookmarksItem
{

public:
    BookmarksItem(QString m_url, QString m_title);
    BookmarksItem(const QDomElement &e);
    QDomElement createXMLNode(QDomDocument &d);
    QString url;
    QString title;
};
#endif // BOOKMARKSITEM_H
