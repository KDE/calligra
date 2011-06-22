#include "KoBalloon.h"

KoBalloon::KoBalloon(QString content, int position, QWidget *parent) :
    QWidget(parent), m_y(position), m_content(content)
{
}

void KoBalloon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush(Qt::green);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBackground(brush);
}
