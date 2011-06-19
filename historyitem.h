#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QString>
#include <QDomElement>
#include <QDomDocument>

class HistoryItem
{

public:
    HistoryItem();
    HistoryItem(QString m_url, QString m_title, QString m_lastVisited);
    HistoryItem(const QDomElement &e);
    QDomElement createXMLNode(QDomDocument &d);
    QString url;
    QString title;
    QString lastVisited;

};

#endif // HISTORYITEM_H
