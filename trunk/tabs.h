#ifndef QTABS_H
#define QTABS_H

#include "browser.h"
#include "tabbar.h"
#include "tab.h"
#include "browserview.h"
#include <QStackedWidget>
#include <QList>
#include <QPushButton>
#include <QScrollArea>
#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>

class BrowserView;
class Tab;
class TabBar;
class browser;
class Tabs : public QStackedWidget
{
    Q_OBJECT

public:
    Tabs(QWidget *parent = 0, TabBar *tabBar = 0);
    ~Tabs();
    void addTab(Tab *tab);
    void removeTab(int index);
    void removeOneTab(Tab *tab);
    void restoreTab();
    void setTabText(int index, QString text);
    void setTabUrl(int index, QString url);
    void setTabIcon(int index, QIcon icon);
    void refreshAll();
    void clear();
    BrowserView *getCurrBrowser();
    Tab *getTab(int index);
    int getTabIdx(BrowserView *web);
    TabBar *tabBar;
    QMap<QString, int> *tabHistory;
    bool canClose;

public slots:
    void setCurrentIndex(int index);
};

#endif // QTABS_H
