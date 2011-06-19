#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include <QUrl>
#include "browserview.h"

class BrowserView;
class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = 0);
    BrowserView *getBrowser();
    QUrl finalUrl; //For restoring tabs only.
    bool privateBrowsing;
};

#endif // TAB_H
