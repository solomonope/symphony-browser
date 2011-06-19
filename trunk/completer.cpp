#include "completer.h"
#include <QtGui>
#include <QtCore>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QModelIndex>
#include "qDebug.h"

Completer::Completer(QObject *parent, QStringList *stringList, CompletionBox *CompletionBox, int minCharCount, bool sensitive) //Constructor. Sets private vars and connects a couple signals to slots
    : QObject(parent)
{
    m_stringList = stringList;
    m_completionBox = CompletionBox;
    m_minCharCount = minCharCount;
    m_sensitive = sensitive;
    connect(m_completionBox, SIGNAL(textChanged(QString)), SLOT(updateCurrList(QString)));
    connect(m_completionBox, SIGNAL(editingFinished()), SLOT(removeFocus()));
}

void Completer::addItem(QString str) //Add item to m_stringList
{
    m_stringList->insert(0, str);
}

void Completer::removeItem(QString str) //Remove specific item from m_stringList
{
    m_stringList->removeOne(str);
}

void Completer::removeItemAt(int index) //Remove item at index from m_stringList
{
    m_stringList->removeAt(index);
}

void Completer::setFullList(QStringList *strList) //Set m_stringList
{
    m_stringList = strList;
}

QStringList* Completer::getFullList() //Return m_stringList
{
    return m_stringList;
}

QStringList Completer::getAllExisting(QString str) //Returns QStringList filled with valid completions
{
    QStringList tmp;
    for (int i = 0; i < getFullList()->count(); i++)
    {
        if (m_sensitive) {
            if (QString(getFullList()->at(i)).mid(str.length()) == str)
                tmp.insert(0, QString(getFullList()->at(i)));
        }
        else {
            if (QString(getFullList()->at(i)).contains(str))
                tmp.insert(0, QString(getFullList()->at(i)));
        }

    }
    return tmp;
}

void Completer::updateCurrList(QString str) //Updates current completion list strings and such .. str = Current text in QCompletionBox
{
    if (!(m_completionBox->hasFocus()))
        return;

    m_completionBox->removeAllCompletionItems();

    if (!(str.length() >= m_minCharCount)) {
        m_completionBox->hidePopup();
        return;
    }

    QStringList tmpStrList = getAllExisting(str);

    if (!(tmpStrList.count() > 0)) {
        m_completionBox->hidePopup();
        return;
    }

    for (int a = 0; a < tmpStrList.count(); a++)
        m_completionBox->addCompletionItem(a, tmpStrList.at(a));

    m_completionBox->showPopup();
}

void Completer::removeFocus() //If no focus, then hide popup
{
    if (!m_completionBox->hasFocus() && !m_completionBox->popup->hasFocus())
        m_completionBox->hidePopup();
}
