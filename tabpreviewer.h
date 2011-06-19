#ifndef TABPREVIEWER_H
#define TABPREVIEWER_H

#include <QLabel>
#include <QWidget>
#include <QPaintEvent>
#include <QString>
#include <QPainter>

class TabPreviewer : public QLabel
{
    Q_OBJECT

public:
    TabPreviewer(QWidget *parent, Qt::WindowFlags f);
    QString txt;

protected:
    void paintEvent(QPaintEvent *pe);
};

#endif // TABPREVIEWER_H
