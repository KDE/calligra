#ifndef FAKE_KNOTIFYCONFIGWIDGET_H
#define FAKE_KNOTIFYCONFIGWIDGET_H

#include <QWidget>

class KNotifyConfigWidget : public QWidget
{
public:
    KNotifyConfigWidget( QWidget *parent = 0 ) : QWidget(parent) {}
    static void configure( QWidget *parent = 0 ) {}
};

#endif

