#ifndef FAKE_KLISTWIDGET_H
#define FAKE_KLISTWIDGET_H

#include <QListWidget>

class KListWidget : public QListWidget
{
public:
    KListWidget( QWidget *parent = 0 ) : QListWidget(parent) {}
};

#endif
