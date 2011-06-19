#include "bookmarksitem.h"

BookmarksItem::BookmarksItem(const QDomElement &e) //Constructor 1
{
    url = e.attribute("url", "");
    title = e.attribute("title", "");
}

BookmarksItem::BookmarksItem(QString m_url, QString m_title) //Constructor 2
{
    url = m_url;
    title = m_title;
}
QDomElement BookmarksItem::createXMLNode(QDomDocument &d) //Turns QDomDocument in to QDomElement
{
    QDomElement bi = d.createElement( "bookmarksItem" );
    bi.setAttribute("title", title);
    bi.setAttribute("url", url);
    return bi;
}
