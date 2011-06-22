#ifndef KOANNOTATIONSIDEBAR_H
#define KOANNOTATIONSIDEBAR_H

#include <QWidget>
#include "KoBalloon.h"

class KoAnnotationSideBar : public QWidget
{
    Q_OBJECT
public:
    explicit KoAnnotationSideBar(QWidget *parent = 0);

    // add a new annotation to the list
    void addAnnotation(QString content, int position);
    // overridden paint event
    void paintEvent(QPaintEvent *event);

private:
    // set the positions of the balloons relative to eachother and the boundries
    void setPositions();
    void reposition(int index);

private:
    QList<KoBalloon*> *annotations;

};

#endif // KOANNOTATIONSIDEBAR_H
