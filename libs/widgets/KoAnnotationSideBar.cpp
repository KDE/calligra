#include "KoAnnotationSideBar.h"

KoAnnotationSideBar::KoAnnotationSideBar(QWidget *parent) :
    QWidget(parent)
{
    annotations = new QList<KoBalloon>();
}

void KoAnnotationSideBar::addAnnotation(QString content)
{
    // TODO: insert in order of KoBalloon y values
    annotations.append(new KoBalloon(content, this));
}
