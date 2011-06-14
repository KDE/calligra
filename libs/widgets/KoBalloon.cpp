#include "KoBalloon.h"

KoBalloon::KoBalloon(QString content, QWidget *parent) :
    QWidget(parent)
{
    m_content = content;
}

void KoBalloon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(this->x, this->y, this->width, this->height);
}
