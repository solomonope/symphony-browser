#ifndef HISTORY_H
#define HISTORY_H

#include <QDateTime>
#include <QList>
#include <QtXml/QDomElement>
#include <QStringList>
#include "historyitem.h"

class HistoryItem;
class History : public QObject
{
    Q_OBJECT

public:
    History(QObject *parent = 0);
    void addItem(QString title, QString url, QDateTime lastVisited);
    void insertItem(HistoryItem tmp);
    void tryRemove(QString url);
    void doRemoveAt(int i);
    void saveHistoryFile();
    QList<HistoryItem> loadHistory();
    QList<HistoryItem> items();
    QDomElement HistoryItemToNode( QDomDocument &d, const HistoryItem &h );
    QStringList* getUrlList();
    QStringList urlList;

private:
    QList<HistoryItem> m_items;

signals:
    void isReady();

};

#endif // HISTORY_H
