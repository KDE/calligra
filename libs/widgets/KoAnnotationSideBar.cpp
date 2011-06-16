#include "KoAnnotationSideBar.h"

KoAnnotationSideBar::KoAnnotationSideBar(QWidget *parent) :
    QWidget(parent)
{
    annotations = new QList<KoBalloon*>();
}

void KoAnnotationSideBar::addAnnotation(QString content, int position)
{
    KoBalloon *curr;
    int i;
    for(i = 0; i < annotations->size(); ++i)
    {
        curr = annotations->at(i);
        if(curr->y() < position)
        {
            annotations->insert(i, new KoBalloon(content, position, this));
            break;
        }
    }
}

void KoAnnotationSideBar::paintEvent(QPaintEvent *event)
{
    int i;
    KoBalloon *curr;
    QPainter painter(this);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBackground(QBrush(Qt::gray));

    setPositions();

    for(i = 0; i < annotations->size(); ++i)
    {
        curr = annotations->at(i);
        curr->paintEvent(event);
        QPoint anchorConnection((curr->pos()).x() - 30, curr->y());// 30 pixels to the left of current balloon
        painter.drawLine(anchorConnection, curr->pos());
    }
}

void KoAnnotationSideBar::setPositions()
{
    //TODO: algorithm to statically place note balloons
}
