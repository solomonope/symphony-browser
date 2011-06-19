#include <QDateTime>
#include <QMetaType>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomElement>
#include "cookies.h"
#include "qdebug.h"


Cookies::Cookies() //Constructor
{}

void Cookies::loadCookies() //Happens on constructor. Loads all Cookies from file and returns QList<NetworkCookieItem>
{
    QList<NetworkCookieItem> tmp;
    QFile file( "data/cookies.dat");
    if (!file.exists()) {
        if(!file.open(QFile::WriteOnly))
            return;

        file.close();
        return;
    }

      if(!file.open(QFile::ReadOnly))
        return;

      QDomDocument xmlFile( "Cookies" );
      if(!xmlFile.setContent(&file))
        return;

      file.close();

      QDomElement root = xmlFile.documentElement();
      if(root.tagName() != "data")
        return;

      QDomNode n = root.firstChild();
      while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
          if(e.tagName() == "NetworkCookieItem") {
            NetworkCookieItem nci(e);
            tmp.insert(0, nci);
          }
        }

        n = n.nextSibling();
      }
      if (!(tmp.isEmpty()))
        m_items = tmp;
}

void Cookies::insertItem(NetworkCookieItem tmp) //Insert NetworkCookieItem in to m_items
{
    m_items.insert(0, tmp);
}
void Cookies::doRemoveAt(int i) //Call removeAt method in m_items
{
    m_items.removeAt(i);
}
void Cookies::clearAll()
{
    m_items.clear();
}

void Cookies::saveCookiesFile() //Save all Cookies to file
{
    QDomDocument xmlFile("Cookies");
    QDomElement root = xmlFile.createElement("data");
    xmlFile.appendChild(root);
    foreach(NetworkCookieItem cookieItem, items())
        root.appendChild(NetworkCookieItemToNode(xmlFile, cookieItem));

    QFile file("data/cookies.dat");
    if(!file.open(QFile::WriteOnly))
      return;

    QTextStream ts(&file);
    ts << xmlFile.toString();
    file.close();
}

QDomElement Cookies::NetworkCookieItemToNode( QDomDocument &d, const NetworkCookieItem &ci ) //QDomDocument ->  QDomElement
{
   QDomElement nci = d.createElement( "NetworkCookieItem" );
   nci.setAttribute("domain", ci.domain);
   nci.setAttribute("expirationDate", ci.expirationDate.toString());
   nci.setAttribute("name", QString(ci.name));
   nci.setAttribute("path", ci.path);
   nci.setAttribute("value", QString(ci.value));
   nci.setAttribute("httpOnly", ci.httpOnly);
   nci.setAttribute("secure", ci.secure);
   return nci;
}

QList<NetworkCookieItem> Cookies::items() const //Return QList<NetworkCookieItem> [m_items]
{
    return m_items;
}
