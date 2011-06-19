#ifndef TABBAR_H
#define TABBAR_H

#include <QWidget>
#include <QAction>
#include <QList>
#include <QPushButton>
#include <QMenu>
#include <QScrollArea>
#include <QFrame>
#include <QString>
#include <QHBoxLayout>
#include <QSignalMapper>
#include "tabs.h"
#include "tabbutton.h"
#include "tab.h"
#include "tabmenu.h"
#include "tabpreviewer.h"

class Tabs;
class TabButton;
class Tab;
class TabMenu;
class TabPreviewer;
class TabBar : public QWidget
{
    Q_OBJECT

public:
    TabBar(QWidget *parent = 0, Tabs *tabs = 0, QPushButton *addTabBtn = 0);
    void addAt(int index, Tab *tab);
    void removeAt(int index);
    void removeAtTab(Tab *tab);
    void setTab(int index);
    void rightClickTab(int index);
    void setButtonSelected(int index);
    void setButtonHovered(int index);
    void setSelected(Tab *tab);
    void resetStylesheets();
    void refreshTabMenu();
    void refreshTabScrollers();
    void refreshPreviewer();
    void updateLayout(int index, TabButton *tab);
    void resetLayout();
    void clear();
    TabButton* at(Tab *tab);
    int count();
    QList<TabButton*> tabs;
    QSignalMapper *mapper1;
    QSignalMapper *mapper2;
    Tabs *base;
    QPushButton *leftArrow;
    QPushButton *rightArrow;
    QAction *closeTabAct;
    QAction *reloadTabAct1;
    QAction *reloadTabAct2;
    QMenu *rightClickMenu1;
    QMenu *rightClickMenu2;
    QFrame *frame;
    TabMenu *tabMenu;
    QHBoxLayout *tabLayout;
    QScrollArea *tabLayoutScroller;
    TabPreviewer *previewer;
    QString stylesheetDefault;
    QString stylesheetSelected;
    QString stylesheetHovered;
    int selectedTab;
    int hoveredTab;
    int scrollRightOffset;
    int scrollAmount;
    int tabWidth;
    int scrollTabAmount;

private slots:
    void removeTab(int index);
    void reloadTab(int index);
    void tabMenuNavigate(QModelIndex index);
    void openTabMenu();
    void changeTab();
    void scrollTabsLeft();
    void scrollTabsRight();

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void leaveEvent(QEvent *);
};

#endif // TABBAR_H
