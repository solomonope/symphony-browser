#include "tabmenu.h"

TabMenu::TabMenu(QWidget *parent, QPushButton *m_btn)
    : QListWidget(parent)
{
    btn = m_btn;
}

void TabMenu::show()
{
   QListWidget::show();
   setFocus();
}

void TabMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (isVisible())
            hide();
    }
    QListWidget::keyPressEvent(event);
}

void TabMenu::focusOutEvent(QFocusEvent *event)
{
    if (isVisible() && !(btn->rect().contains(btn->mapFromGlobal(QCursor::pos()))))
        hide();
    QListWidget::focusOutEvent(event);
}
