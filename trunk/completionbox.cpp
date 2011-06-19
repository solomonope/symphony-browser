#include "completionbox.h"
#include <QtGui>
#include <QListWidget>
#include "browser.h"
#include "qDebug.h"

CompletionBox::CompletionBox(QWidget *parent, int maxCompletionItems) //Constructor. Set up popup object
    : QLineEdit(parent)
{
    m_maxCompletionItems = maxCompletionItems;
    popup = new ListWidget(this);

    popup->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::Dialog); //No frame/border, always on top, and a dialog[can draw outside of its parent]
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->setAttribute(Qt::WA_ShowWithoutActivating);
    connect(popup, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(doSelect(QListWidgetItem*)));
    connect(popup, SIGNAL(attemptedSelect()), SLOT(doSelect()));
    connect(this, SIGNAL(editingFinished()), SLOT(finished_editing()));
    browser* tmpWind = qobject_cast<browser*>(window());
    if (tmpWind)
        connect(tmpWind, SIGNAL(moved(QPoint)), SLOT(onMove(QPoint)));
}

CompletionBox::~CompletionBox() //Destructor
{
    delete popup;
}


void CompletionBox::addCompletionItem(int index, const QString input) //Add completion item to completionItems
{
    if (completionItems.count() < m_maxCompletionItems)
        completionItems.insert(index, input);
}

void CompletionBox::removeCompletionItem(const QString input) //Removes single specific QString from completionItems
{
    completionItems.removeOne(input);
}

void CompletionBox::removeCompletionItemAt(int index) //Removes QString at index from completionItems
{
    completionItems.removeAt(index);
}

void CompletionBox::removeAllCompletionItems() //Clears all items in completionItems
{
    for(int i = completionItems.count() - 1; i >= 0; i--)
        completionItems.removeAt(i);
}

void CompletionBox::showPopup() //Show the popup
{
    popup->removeAllItems();
    popup->insertItems(0, completionItems);
    popup->move(window()->geometry().x() + geometry().x(), window()->geometry().y() + height());
    popup->resize(width(), (height() * maxCompletionItems()));
    popup->show();
}

void CompletionBox::hidePopup() //Hide the popup
{
    popup->hide();
}

int CompletionBox::maxCompletionItems() //Gets number of completion items to show
{
    if (popup->count() >= m_maxCompletionItems)
        return m_maxCompletionItems;
    else
        return popup->count();
}

void CompletionBox::doSelect(QListWidgetItem *item) //Same as doSelect(), but sets the CompletionBox to the QListWidgetItem text
{
    activateWindow();
    setFocus();
    setText(item->text());
    popup->selectionModel()->clearSelection();
    popup->hide();
}

void CompletionBox::doSelect() //Select and activate CompletionBox whilst deselecting popup
{
    activateWindow();
    setFocus();
    setText(popup->currentItem()->text());
    popup->selectionModel()->clearSelection();
    popup->hide();
}

void CompletionBox::onMove(QPoint e) //On move. For keeping the popup at the correct coordinates
{
    Q_UNUSED(e)
    popup->move(window()->geometry().x() + geometry().x(), window()->geometry().y() + height());
}

void CompletionBox::finished_editing()
{
    if (!(popup->hasFocus()))
        hidePopup();
}


void CompletionBox::keyPressEvent(QKeyEvent *e) //On key press... cleans down arrow when in completion box
{
    QLineEdit::keyPressEvent(e);
    switch(e->key())
    {
    case Qt::Key_Up: //Up arrow
        if(popup->count() > 0) {
            if (!(popup->isVisible()))
                popup->show();
            popup->setFocus();
            popup->activateWindow();
            popup->setCurrentRow(popup->count() - 1);
        }
        break;
    case Qt::Key_Down: //Down arrow
        if(popup->count() > 0) {
            if (!(popup->isVisible()))
                popup->show();

            popup->setFocus();
            popup->activateWindow();
            popup->setCurrentRow(0);
        }
        break;
    case Qt::Key_Escape:
        if (popup->isVisible())
            popup->hide();
    }
}

void CompletionBox::resizeEvent(QResizeEvent *e) //On resize event... for keeping the popup the correct size
{
    QLineEdit::resizeEvent(e);
    popup->move(window()->geometry().x() + geometry().x(), window()->geometry().y() + height());
    popup->resize(width(), popup->height());
}
