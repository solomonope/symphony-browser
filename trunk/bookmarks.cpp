#include <QDateTime>
#include <QMetaType>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomElement>
#include "bookmarks.h"
#include "qdebug.h"


Bookmarks::Bookmarks(QObject *parent) //Constructor
    : QObject(parent)
{
    urlList = new QStringList();
    m_items = loadBookmarks();
}

Bookmarks::~Bookmarks() //Deconstructor
{
    delete urlList;
}

QList<BookmarksItem> Bookmarks::items() const //Return QList<BookmarksItem> [m_items]
{
    return m_items;
}

void Bookmarks::addItem(QString title, QString url) //Add item to m_items and urlList
{
    BookmarksItem tmp(url, title);
    if (!urlList->contains(url))
        urlList->insert(0, url);

    insertItem(tmp);
}

void Bookmarks::insertItem(BookmarksItem tmp) //Insert BookmarksItem in to m_items
{
    m_items.insert(0, tmp);
}

void Bookmarks::doRemoveAt(int i) //Call removeAt method in m_items
{
    m_items.removeAt(i);
}

void Bookmarks::saveBookmarksFile() //Save all bookmarks to file
{
    QDomDocument xmlFile("bookmarks");
    QDomElement root = xmlFile.createElement("data");
    xmlFile.appendChild(root);
    foreach(BookmarksItem histItem, items())
        root.appendChild(BookmarksItemToNode(xmlFile, histItem));

    QFile file("data/bookmarks.dat");
    if(!file.open(QFile::WriteOnly))
      return;

    QTextStream ts(&file);
    ts << xmlFile.toString();
    file.close();
}

QList<BookmarksItem> Bookmarks::loadBookmarks() //Happens on constructor. Loads all bookmarks from file and returns QList<BookmarksItem>
{
    QList<BookmarksItem> tmp;
    QFile file( "data/bookmarks.dat");
    if (!file.exists()) {
        if(!file.open(QFile::WriteOnly))
            return tmp;

        file.close();
        return tmp;
    }

      if(!file.open(QFile::ReadOnly))
        return tmp;

      QDomDocument xmlFile( "bookmarks" );
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
          if(e.tagName() == "bookmarksItem") {
            BookmarksItem bi(e);
            tmp.insert(0, bi);
          }
        }

        n = n.nextSibling();
      }
      return tmp;
}

QDomElement Bookmarks::BookmarksItemToNode( QDomDocument &d, const BookmarksItem &b ) //QDomDocument ->  QDomElement
{
   QDomElement bi = d.createElement( "bookmarksItem" );
   bi.setAttribute("title", b.title);
   bi.setAttribute("url", b.url);
   return bi;
}
