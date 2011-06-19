#ifndef COMPLETER_H
#define COMPLETER_H
#include <QtGui>
#include <QtCore>
#include "completionbox.h"

class CompletionBox;

class Completer : public QObject
{
    Q_OBJECT

public:
    Completer(QObject *parent = 0, QStringList *stringList = 0, CompletionBox *CompletionBox = 0, int minCharCount = 3, bool sensitive = false);
    void addItem(QString str);
    void removeItem(QString str);
    void removeAt(int index);
    void removeItemAt(int index);
    void setFullList(QStringList *strList);
    QStringList* getFullList();
    QStringList getAllExisting(QString str);

private:
    CompletionBox *m_completionBox;
    QStringList *m_stringList;
    int m_minCharCount;
    bool m_sensitive;

private slots:
    void updateCurrList(QString str);
    void removeFocus();
};

#endif // COMPLETER_H
