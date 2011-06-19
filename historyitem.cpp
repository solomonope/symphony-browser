#include "historyitem.h"

HistoryItem::HistoryItem()
{}


HistoryItem::HistoryItem(const QDomElement &e) //Constructor. Sets up private vars as per QDomElement
{
    url = e.attribute("url", "");
    lastVisited = e.attribute("lastVisit", "");
    title = e.attribute("title", "");
}

HistoryItem::HistoryItem(QString m_url, QString m_title, QString m_lastVisited) //Constructor. Sets up private vars as per QStrings
{
    url = m_url;
    title = m_title;
    lastVisited = m_lastVisited;
}

QDomElement HistoryItem::createXMLNode(QDomDocument &d) //Creates XML nodes with QDomDocument - returns QDomElement
{
    QDomElement hi = d.createElement( "historyItem" );
    hi.setAttribute("title", title);
    hi.setAttribute("url", url);
    hi.setAttribute("lastVisit", lastVisited);
    return hi;
}
