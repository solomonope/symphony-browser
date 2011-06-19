#include "tabbar.h"

TabBar::TabBar(QWidget *parent, Tabs *m_tabs, QPushButton *addTabBtn)
    : QWidget(parent)
{
    base = m_tabs;

    scrollRightOffset = 0;
    tabWidth = 24;
    scrollTabAmount = 5;
    scrollAmount = 1;

    addTabBtn->setMouseTracking(true);

    QPushButton *menuBtn = new QPushButton();
    menuBtn->setMouseTracking(true);
    menuBtn->resize(31, 24);
    menuBtn->setMaximumSize(31, 24);
    menuBtn->setStyleSheet("QPushButton { margin-top: 10px; border: 0; border-radius: 8px; } QPushButton:pressed { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D3D3D3, stop: 0.4 #BABABA, stop: 0.5 #AAAAAA, stop: 1.0 #CDCDCD); }");
    menuBtn->setIcon(QIcon(":/icons/tabmenu.png"));
    connect(menuBtn, SIGNAL(clicked()), SLOT(openTabMenu()));
    tabMenu = new TabMenu(this, menuBtn);
    tabMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::Dialog); //No frame/border, always on top, and a dialog[can draw outside of its parent]
    tabMenu->setStyleSheet("border-radius: 7px; border: 1px solid #000000; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);");
    tabMenu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tabMenu->resize(200, 225);
    connect(tabMenu, SIGNAL(doubleClicked(QModelIndex)), SLOT(tabMenuNavigate(QModelIndex)));

    QHBoxLayout *baseLayout = new QHBoxLayout();
    baseLayout->setMargin(0);
    baseLayout->setSpacing(1);
    baseLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QSpacerItem *spacer = new QSpacerItem(3, 0, QSizePolicy::Fixed);
    baseLayout->addItem(spacer);

    baseLayout->addWidget(menuBtn);
    setLayout(baseLayout);

    baseLayout->addWidget(addTabBtn);

    rightArrow = new QPushButton();
    leftArrow = new QPushButton();
    rightArrow->setMouseTracking(true);
    leftArrow->setMouseTracking(true);
    rightArrow->resize(16, 24);
    leftArrow->resize(16, 24);
    leftArrow->setMaximumSize(16, 24);
    rightArrow->setMaximumSize(16, 24);
    leftArrow->setIcon(QIcon(":/icons/left.png"));
    rightArrow->setIcon(QIcon(":/icons/right.png"));
    leftArrow->setStyleSheet("QPushButton { border: 0; margin-top: 6px; border-radius: 7px; } QPushButton:pressed {  background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); }");
    rightArrow->setStyleSheet("QPushButton { border: 0; margin-top: 6px; border-radius: 7px; } QPushButton:pressed {  background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); }");
    leftArrow->setAutoRepeat(true);
    leftArrow->setAutoRepeatInterval(1);
    leftArrow->setAutoRepeatDelay(1);
    rightArrow->setAutoRepeat(true);
    rightArrow->setAutoRepeatInterval(1);
    rightArrow->setAutoRepeatDelay(1);
    connect(rightArrow, SIGNAL(pressed()), SLOT(scrollTabsRight()));
    connect(leftArrow, SIGNAL(pressed()), SLOT(scrollTabsLeft()));
    baseLayout->addWidget(leftArrow);
    baseLayout->addWidget(rightArrow);

    QSpacerItem *spacerBig = new QSpacerItem(5, 0, QSizePolicy::Fixed);
    baseLayout->addItem(spacerBig);

    QScrollArea *scroll = new QScrollArea();
    scroll->setMouseTracking(true);
    scroll->setStyleSheet("border: 0;");
    scroll->setMaximumHeight(24);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->setSpacing(8);
    QFrame *frame = new QFrame();
    frame->setMouseTracking(true);
    frame->setStyleSheet("border: 0;");
    frame->setMaximumHeight(24);
    frame->setLayout(layout);
    scroll->setWidget(frame);

    tabLayoutScroller = scroll;
    tabLayout = layout;
    baseLayout->addWidget(scroll);

    setMouseTracking(true);

    mapper1 = new QSignalMapper(this);
    mapper2 = new QSignalMapper(this);

    selectedTab = 0;
    hoveredTab = -1;

    stylesheetDefault.append("background: #b3b3b3;");
    stylesheetDefault.append("border-top: 1px solid #666666;");
    stylesheetDefault.append("border-left: 1px solid #666666;");
    stylesheetDefault.append("border-right: 1px solid #666666;");
    stylesheetDefault.append("border-top-right-radius: 7px;");
    stylesheetDefault.append("border-top-left-radius: 7px;");

    stylesheetSelected.append("background: #808080;");
    stylesheetSelected.append("border-top: 2px solid #3b3b3b;");
    stylesheetSelected.append("border-left: 2px solid #3b3b3b;");
    stylesheetSelected.append("border-right: 2px solid #3b3b3b;");
    stylesheetSelected.append("border-top-right-radius: 7px;");
    stylesheetSelected.append("border-top-left-radius: 7px;");

    stylesheetHovered.append("background: #ebebeb;");
    stylesheetHovered.append("border-top: 1px solid #a3a3a3;");
    stylesheetHovered.append("border-left: 1px solid #a3a3a3;");
    stylesheetHovered.append("border-right: 1px solid #a3a3a3;");
    stylesheetHovered.append("border-top-right-radius: 7px;");
    stylesheetHovered.append("border-top-left-radius: 7px;");

    rightClickMenu1 = new QMenu(this);
    rightClickMenu2 = new QMenu(this);
    closeTabAct = rightClickMenu1->addAction("Close Tab", mapper1, SLOT(map()));
    reloadTabAct1 = rightClickMenu1->addAction("Reload Tab", mapper2, SLOT(map()));
    reloadTabAct2 = rightClickMenu2->addAction("Reload Tab", mapper2, SLOT(map()));
    connect(mapper1, SIGNAL(mapped(int)), this, SLOT(removeTab(int)));
    connect(mapper2, SIGNAL(mapped(int)), this, SLOT(reloadTab(int)));

    previewer = new TabPreviewer(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::Dialog);
    previewer->hide();
    previewer->setStyleSheet("border: 1px solid black;");
    previewer->resize(267, 215); //800x600 / 3 (+ 15 to height for title)
}

void TabBar::addAt(int index, Tab *tab)
{
    TabButton *button = new TabButton(this, tab);
    button->setMouseTracking(true);
    button->resize(24, 24);
    button->setMaximumSize(24, 24);
    button->setStyleSheet(stylesheetDefault);
    connect(button, SIGNAL(pressed()), SLOT(changeTab()));
    updateLayout(index, button);
    tabs.insert(index, button);
    setTab(count() - 1);
    if (tabMenu->isVisible())
        refreshTabMenu();
    refreshTabScrollers();
}

void TabBar::removeAt(int index)
{
    TabButton *button = tabs.at(index);
    bool selected = button->selected;

    updateLayout(-1, button);

    base->removeOneTab(button->tab);

    tabs.removeOne(button);
    delete button;

    if (selected) {
        if (index == 0) {
            selectedTab = 0;
            setButtonSelected(0);
        }
        else {
            selectedTab = index - 1;
            setButtonSelected(index - 1);
        }
    }

    if (tabMenu->isVisible())
        refreshTabMenu();

    refreshTabScrollers();
}

void TabBar::removeAtTab(Tab *tab)
{
    for(int i = 0; i < tabs.count(); i++) {
        if (tabs.at(i)->tab == tab) {
            removeAt(i);
            break;
        }
    }
}

void TabBar::setTab(int index)
{
    TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(index)->widget());
    base->setCurrentWidget(button->tab);
    setButtonSelected(index);
}

void TabBar::rightClickTab(int index)
{

    if (count() > 1) {
        mapper1->setMapping(closeTabAct, index);
        mapper2->setMapping(reloadTabAct1, index);
        rightClickMenu1->popup(tabLayout->itemAt(index)->widget()->mapToGlobal(tabLayout->itemAt(index)->widget()->rect().bottomLeft()));
    }
    else {
        mapper2->setMapping(reloadTabAct2, index);
        rightClickMenu2->popup(tabLayout->itemAt(index)->widget()->mapToGlobal(tabLayout->itemAt(index)->widget()->rect().bottomLeft()));
    }
}

void TabBar::setButtonSelected(int index)
{
    selectedTab = index;
    for (int i = 0; i < tabLayout->count(); i++) {
        TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
        if (i == index) {
            button->selected = true;
            button->setStyleSheet(stylesheetSelected);
        }
        else if (button->selected == true) {
                button->selected = false;
                button->setStyleSheet(stylesheetDefault);
        }
    }
    if (tabLayoutScroller->horizontalScrollBar()->maximum() > 0)
        tabLayoutScroller->horizontalScrollBar()->setValue(tabLayoutScroller->horizontalScrollBar()->maximum());
}

void TabBar::setButtonHovered(int index)
{
    hoveredTab = index;
    for (int i = 0; i < tabLayout->count(); i++) {
        TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
        if (i == index) {
            button->hovered = true;
            button->setStyleSheet(stylesheetHovered);
        }
        else if (button->hovered == true) {
                button->hovered = false;
                if (!(button->selected))
                    button->setStyleSheet(stylesheetDefault);
                else
                    button->setStyleSheet(stylesheetSelected);
        }
    }
}

void TabBar::setSelected(Tab *tab)
{
    int i;
    for (i = 0; i < tabLayout->count(); i++) {
        TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
        if (button->tab == tab)
            break;
    }
    selectedTab = i;
    setButtonSelected(i);
}

void TabBar::resetStylesheets()
{
    setButtonHovered(-1);
    setButtonSelected(selectedTab);
}

void TabBar::refreshTabMenu()
{
    tabMenu->clear();
    if (count() > 1)
        tabMenu->insertItem(0, "Close Current Tab");
    QString iStr;
    foreach(TabButton *button, tabs) {
        iStr.setNum(tabMenu->count());
        tabMenu->insertItem(tabMenu->count(), "[Tab " + iStr + "]: " + button->title);
    }
}


void TabBar::refreshTabScrollers()
{
    if (tabLayoutScroller->horizontalScrollBar()->maximum() > 0) {
     rightArrow->show();
     leftArrow->show();
    }
    else {
     rightArrow->hide();
     leftArrow->hide();
    }
}

void TabBar::refreshPreviewer()
{
    if (previewer->isVisible()) {
        previewer->txt = tabs.at(hoveredTab)->title;
        previewer->repaint();
    }
}

void TabBar::updateLayout(int index, TabButton *tab)
{

    tabLayoutScroller->setUpdatesEnabled(false);

    if (index == -1) {
        tab->setIcon(QIcon());
        tabLayout->removeWidget(tab);
    }
    else {
        tabLayout->addWidget(tab);
    }

    tabLayoutScroller->setUpdatesEnabled(true);

    tabLayoutScroller->update();
    tabLayoutScroller->repaint();
}

void TabBar::resetLayout()
{
    QLayoutItem *child;
    while ((child = tabLayout->takeAt(0)) != 0)
    delete child;
}

void TabBar::clear()
{
    tabs.clear();
    resetLayout();
}

TabButton* TabBar::at(Tab *tab)
{
    foreach(TabButton *btn, tabs) {
        if (btn->tab == tab)
            return btn;
    }

    return 0;
}

int TabBar::count()
{
    return tabs.count();
}

void TabBar::removeTab(int index)
{
    removeAt(index);
}

void TabBar::reloadTab(int index)
{
    TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(index)->widget());
    button->tab->getBrowser()->reload();
}

void TabBar::tabMenuNavigate(QModelIndex index)
{
    if (index.row() == 0 && count() > 1) {
        removeAt(selectedTab);
        return;
    }

    TabButton *button;
    if (count() > 1)
        button = qobject_cast<TabButton*>(tabLayout->itemAt(index.row() - 1)->widget());
    else
        button = qobject_cast<TabButton*>(tabLayout->itemAt(index.row())->widget());

    for (int i = 0; i < tabs.count(); i++)
    {
        TabButton *btn = tabs.at(i);
        if (btn == button) {
            setTab(i);
            break;
        }
    }
}

void TabBar::openTabMenu()
{
    if (tabMenu->isVisible()) {
        tabMenu->hide();
        return;
    }
    tabMenu->move(mapToGlobal(QPoint(tabLayout->geometry().x(), 0)).x(), mapToGlobal(QPoint(tabLayout->geometry().y(), 0)).y() + tabLayout->contentsRect().height());
    tabMenu->clear();
    QString iStr;
    if (count() > 1)
        tabMenu->insertItem(0, "Close Current Tab");
    foreach(TabButton *button, tabs) {
        iStr.setNum(tabMenu->count());
        tabMenu->insertItem(tabMenu->count(), "[Tab " + iStr + "]: " + button->title);
    }
    tabMenu->show();
}

void TabBar::changeTab()
{
    for (int i = 0; i < tabLayout->count(); i++) {
        TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
        QPoint mouse = button->mapFromGlobal(QCursor::pos());
        if (button->rect().contains(mouse)) {
            setButtonSelected(i);
            base->setCurrentWidget(button->tab);
            break;
        }

    }
}

void TabBar::scrollTabsLeft()
{
    if (tabLayoutScroller->horizontalScrollBar()->value() - 1 >= tabLayoutScroller->horizontalScrollBar()->minimum()) {
        if (tabLayoutScroller->horizontalScrollBar()->value() - 3 >= tabLayoutScroller->horizontalScrollBar()->minimum())
            tabLayoutScroller->horizontalScrollBar()->setValue(tabLayoutScroller->horizontalScrollBar()->value() - 3);
        else
            tabLayoutScroller->horizontalScrollBar()->setValue(tabLayoutScroller->horizontalScrollBar()->value() - 1);
    }
}

void TabBar::scrollTabsRight()
{
    if (tabLayoutScroller->horizontalScrollBar()->value() + 1 <= tabLayoutScroller->horizontalScrollBar()->maximum()) {
        if ((tabLayoutScroller->horizontalScrollBar()->value() + 3 <= tabLayoutScroller->horizontalScrollBar()->maximum()))
            tabLayoutScroller->horizontalScrollBar()->setValue(tabLayoutScroller->horizontalScrollBar()->value() + 3);
        else
            tabLayoutScroller->horizontalScrollBar()->setValue(tabLayoutScroller->horizontalScrollBar()->value() + 1);
    }
}



void TabBar::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        for (int i = 0; i < tabLayout->count(); i++) {
            TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
            QPoint mouse = button->mapFromGlobal(QCursor::pos());
            if (button->rect().contains(mouse)) {
                rightClickTab(i);
                break;
            }
        }
    }
    else
        QWidget::mousePressEvent(e);
}



void TabBar::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        if (tabMenu->isVisible())
            tabMenu->hide();
    }
    QWidget::keyPressEvent(e);
}

void TabBar::mouseMoveEvent(QMouseEvent *e)
{
    int notHovered = 0;
    for (int i = 0; i < tabLayout->count(); i++) {
        TabButton *button = qobject_cast<TabButton*>(tabLayout->itemAt(i)->widget());
        QPoint mouse = button->mapFromGlobal(QCursor::pos());
        QString iStr = "";
        iStr.setNum(i);
        QRect buttonRect(button->mapToGlobal(button->rect().topLeft()), button->mapToGlobal(button->rect().bottomRight()));
        QRect baseRect(tabLayoutScroller->mapToGlobal(rect().topLeft()), tabLayoutScroller->mapToGlobal(rect().bottomRight()));
        if (button->rect().contains(mouse) && baseRect.contains(buttonRect)) {
            hoveredTab = i;
            setButtonHovered(i);
            if (button->tab->size() != base->getTab(base->currentIndex())->size()) { //Workaround for widgets being very tiny if user hasn't viewed them yet
                button->tab->resize(base->getTab(base->currentIndex())->size());
                button->tab->getBrowser()->resize(base->getCurrBrowser()->size());
            }
            QPixmap preview = QPixmap::grabWidget(button->tab->getBrowser());
            previewer->move(button->mapToGlobal(button->rect().bottomLeft()));
            QFont font = previewer->font();
            font.setBold(true);
            previewer->setFont(font);
            int vertFontSpacing = 15;
            previewer->setAlignment(Qt::AlignBottom);
            previewer->setPixmap(preview.scaled(previewer->size().width(), previewer->size().height() - vertFontSpacing));
            previewer->txt = button->title;
            previewer->repaint();
            previewer->show();
            break;
        }
        else
            notHovered++;
    }

    if (notHovered == tabLayout->count()) {
        if (previewer->isVisible()) {
            previewer->setPixmap(QPixmap());
            previewer->hide();
        }
        setButtonHovered(-1);
    }

    QWidget::mouseMoveEvent(e);
}

void TabBar::leaveEvent(QEvent *)
{
    if (previewer->isVisible()) {
        previewer->setPixmap(QPixmap());
        previewer->hide();
    }
    resetStylesheets();
}
