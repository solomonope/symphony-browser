#include "tabbutton.h"

TabButton::TabButton(QWidget *parent, Tab *m_tab)
    : QPushButton(parent)
{
    tab = m_tab;
    hovered = false;
    selected = false;
}
