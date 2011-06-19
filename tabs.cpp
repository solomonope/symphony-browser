#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QPixmap>
#include "tabs.h"
#include "qdebug.h"

Tabs::Tabs(QWidget *parent, TabBar *m_tabBar) //Constructor.
    : QStackedWidget(parent)
{
    tabHistory = new QMap<QString, int>();
    tabBar = m_tabBar;
    if (count() > 1)
        canClose = true;
    else
        canClose = false;
}

Tabs::~Tabs() //Deconstructor
{
    delete tabHistory;
}

void Tabs::addTab(Tab *tab) //Adds tab. Requires title of tab [&str] and the Tab itself
{
    QStackedWidget::addWidget(tab);

    tabBar->addAt(tabBar->count(), tab);

    if (count() > 1)
        canClose = true;
    else
        canClose = false;
}

void Tabs::removeTab(int index) //Handles closing tabs. Inserts tab in to tabHistory if tab is not private.
{
    if (!(getTab(index)->privateBrowsing))
        tabHistory->insert(getTab(index)->getBrowser()->url().toString(), tabHistory->count());

    getTab(index)->getBrowser()->deleteLater();

    Tab *t = getTab(index);
    QStackedWidget::removeWidget(t);

    delete t;

    tabBar->removeAtTab(getTab(index));

    if (count() > 1)
        canClose = true;
    else
        canClose = false;
}

void Tabs::removeOneTab(Tab *tab)
{
    tab->getBrowser()->deleteLater();
    QStackedWidget::removeWidget(tab);
    delete tab;
}

void Tabs::restoreTab() //Restores tab when the restore last tab function is used
{
    if (tabHistory->isEmpty())
        return;

    QString url = tabHistory->key(tabHistory->count() - 1);
    tabHistory->remove(tabHistory->keys().last());

    browser *wind = qobject_cast<browser*>(window());
    wind->addTab(url, false);
}

void Tabs::setTabText(int index, QString text)
{
    tabBar->at(getTab(index))->title = text;
    tabBar->refreshPreviewer();
    if (tabBar->tabMenu->isVisible())
        tabBar->refreshTabMenu();
}

void Tabs::setTabUrl(int index, QString url)
{
    tabBar->at(getTab(index))->url = url;
}

void Tabs::setTabIcon(int index, QIcon icon)
{
    tabBar->at(getTab(index))->setIcon(icon);
}

void Tabs::refreshAll()
{
    for(int i = 0; i < count(); i++)
        getTab(i)->getBrowser()->reload();
}

void Tabs::clear()
{
    tabBar->clear();
}

BrowserView* Tabs::getCurrBrowser() //Gets current tab's BrowserView
{
    if (currentIndex() != -1) {
        QWidget *currTab = widget(currentIndex());
        return qobject_cast<BrowserView*>(currTab->children().at(1));
    }
    return 0;
}

Tab* Tabs::getTab(int index) //Returns Tab at index
{
    return qobject_cast<Tab*>(widget(index));
}

int Tabs::getTabIdx(BrowserView *web) //Gets index of tab where inputted BrowserView is located
{
    for(int i = 0; i < count(); i++) {
        if (getTab(i)->getBrowser() == web)
            return i;
    }

    return -1;
}

void Tabs::setCurrentIndex(int index)
{
    QStackedWidget::setCurrentIndex(index);
    tabBar->setSelected(getTab(index));
}


