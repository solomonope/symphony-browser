#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>

class ListWidget : public QListWidget
{
    Q_OBJECT

public:
    ListWidget(QWidget *parent = 0);
    void removeAllItems();

private:
    QLineEdit* m_CompletionBox;

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void attemptedSelect();

};

#endif // LISTWIDGET_H
