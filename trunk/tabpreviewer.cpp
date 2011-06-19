#include "tabpreviewer.h"

TabPreviewer::TabPreviewer(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{}

void TabPreviewer::paintEvent(QPaintEvent *pe)
{
    QLabel::paintEvent(pe);
    QPainter p(this);
    p.setPen(QColor::fromRgb(0, 0, 0, 255));
    p.drawText(QPoint(1, 11), txt);
}
