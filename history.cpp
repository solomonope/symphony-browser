#include <QDateTime>
#include <QMetaType>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomElement>
#include "history.h"
#include "qdebug.h"


History::History(QObject *parent) //Constructor
    : QObject(parent)
{
    m_items = loadHistory();
}

void History::addItem(QString title, QString url, QDateTime lastVisited)
{
    HistoryItem tmp(url, title, lastVisited.date().toString());
    if (!urlList.contains(url))
        urlList.insert(urlList.count(), url);

    tryRemove(url);
    insertItem(tmp);
}

void History::insertItem(HistoryItem tmp)
{
    m_items.insert(0, tmp);
}

void History::tryRemove(QString url)
{
    for(int i = 0; i < items().count(); i++) {
        if (items().at(i).url == url) {
            doRemoveAt(i);
            break;
        }
    }
}

void History::doRemoveAt(int i)
{
    m_items.removeAt(i);
}

void History::saveHistoryFile()
{
    QDomDocument xmlFile("history");
    QDomElement root = xmlFile.createElement("data");
    xmlFile.appendChild(root);
    QList<HistoryItem> tmpList = items();
    foreach(HistoryItem histItem, tmpList)
        root.appendChild(HistoryItemToNode(xmlFile, histItem));

    QFile file("data/history.dat");
    if(!file.open(QFile::WriteOnly))
      return;

    QTextStream ts(&file);
    ts << xmlFile.toString();
    file.close();
}

QList<HistoryItem> History::loadHistory() //Loads history from file and returns QList<HistoryItem>
{
    QList<HistoryItem> tmp;
    QFile file( "data/history.dat");
    if (!file.exists()) {
        if(!file.open(QFile::WriteOnly))
            return tmp;

        file.close();
        return tmp;
    }

      if(!file.open(QFile::ReadOnly))
        return tmp;

      QDomDocument xmlFile( "history" );
      if(!xmlFile.setContent(&file))
        return tmp;

      file.close();

      QDomElement root = xmlFile.documentElement();
      if(root.tagName() != "data")
        return tmp;

      QDomNode n = root.firstChild();
      while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
          if(e.tagName() == "historyItem") {
            HistoryItem hi(e);
            urlList.insert(urlList.count(), hi.url);
            tmp.insert(tmp.count(), hi);
          }
        }

        n = n.nextSibling();
      }
      return tmp;
}

QList<HistoryItem> History::items()
{
    return m_items;
}

QDomElement History::HistoryItemToNode( QDomDocument &d, const HistoryItem &h )
{
   QDomElement hi = d.createElement( "historyItem" );
   hi.setAttribute("title", h.title);
   hi.setAttribute("url", h.url);
   hi.setAttribute("lastVisit", h.lastVisited);
   return hi;
}

QStringList* History::getUrlList()
{
    return &urlList;
}
