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
    void addAnnotation(QString content);

private:
    QList<KoBalloon*> annotations;

};

#endif // KOANNOTATIONSIDEBAR_H
