#ifndef TABMENU_H
#define TABMENU_H

#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <QKeyEvent>
#include <QFocusEvent>

class TabMenu : public QListWidget
{
    Q_OBJECT

public:
    TabMenu(QWidget *parent = 0, QPushButton *btn = 0);
    QPushButton *btn;

public slots:
    void show();

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
};

#endif // TABMENU_H
