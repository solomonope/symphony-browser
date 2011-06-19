#include "listwidget.h"
#include <QKeyEvent>
#include <QLineEdit>


ListWidget::ListWidget(QWidget *parent) //Constructor.
    : QListWidget(parent)
{
    m_CompletionBox = qobject_cast<QLineEdit*>(parent);
    setVisible(false);
}

void ListWidget::removeAllItems() //Remove all items in ListWidget
{
    for (int i = count(); i >= 0; i--)
        delete takeItem(i);
}

void ListWidget::keyPressEvent(QKeyEvent *event) //On key press. Handles enter key, up arrow, down arrow.
{
    switch (event->key()) {
    case Qt::Key_Enter: //Enter/return key
    case Qt::Key_Return:
        emit attemptedSelect();
        break;
    case Qt::Key_Up: //Up arrow
        if (currentRow() != 0)
            setCurrentRow(currentRow());
        else {
            selectionModel()->clearSelection();
            m_CompletionBox->activateWindow();
            m_CompletionBox->setFocus();
        }
        break;
    case Qt::Key_Down: //Down arrow
        if (currentRow() != count() - 1)
            setCurrentRow(currentRow());
        else {
            selectionModel()->clearSelection();
            m_CompletionBox->activateWindow();
            m_CompletionBox->setFocus();
        }
        break;
    case Qt::Key_Escape:
        hide();
        break;
    }
    QListWidget::keyPressEvent(event);
}
