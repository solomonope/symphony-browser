#ifndef CompletionBox_H
#define CompletionBox_H

#include <QtGui>
#include <QAbstractItemModel>
#include "listwidget.h"

class ListWidget;

class CompletionBox : public QLineEdit
{
    Q_OBJECT

public:
    CompletionBox(QWidget *parent = 0, int maxCompletionItems = 8);
    ~CompletionBox();
    void addCompletionItem(int index, const QString input);
    void removeCompletionItem(const QString input);
    void removeCompletionItemAt(int index);
    void removeAllCompletionItems();
    void showPopup();
    void hidePopup();
    int maxCompletionItems();
    ListWidget *popup;
    QStringList completionItems;

private:
    int m_maxCompletionItems;

private slots:
    void doSelect(QListWidgetItem*);
    void doSelect();
    void onMove(QPoint e);
    void finished_editing();

protected:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);

};

#endif // CompletionBox_H
