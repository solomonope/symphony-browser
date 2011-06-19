#ifndef TABBUTTON_H
#define TABBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QString>
#include "tab.h"

class TabButton : public QPushButton
{
    Q_OBJECT

public:
    TabButton(QWidget *parent = 0, Tab *tab = 0);
    Tab *tab;
    QString title;
    QString url;
    bool selected;
    bool hovered;
};

#endif // TABBUTTON_H
