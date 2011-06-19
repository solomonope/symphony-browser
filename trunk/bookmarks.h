#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QDateTime>
#include <QList>
#include <QtXml/QDomElement>
#include "bookmarksitem.h"

class BookmarksItem;
class Bookmarks : public QObject
{
    Q_OBJECT

public:
    Bookmarks(QObject *parent = 0);
    ~Bookmarks();
    QList<BookmarksItem> items() const;
    void addItem(QString title, QString url);
    void insertItem(BookmarksItem tmp);
    void doRemoveAt(int i);
    void saveBookmarksFile();
    QList<BookmarksItem> loadBookmarks();
    QDomElement BookmarksItemToNode( QDomDocument &d, const BookmarksItem &h );
    QStringList *urlList;


private:
    QList<BookmarksItem> m_items;

};

#endif // BOOKMARKS_H
