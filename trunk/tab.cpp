#include "tab.h"

Tab::Tab(QWidget *parent) : //[Empty] constructor
    QWidget(parent)
{
}

BrowserView* Tab::getBrowser() //Returns BrowserView of current tab
{
    if (children().count() > 1)
        return qobject_cast<BrowserView*>(children().at(1));
    else
        return 0;
}
