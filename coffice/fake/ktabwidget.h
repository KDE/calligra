#ifndef FAKE_KTABWIDGET_H
#define FAKE_KTABWIDGET_H

#include <QTabWidget>

class KTabWidget : public QTabWidget
{
public:
    KTabWidget(QWidget *parent = 0) : QTabWidget(parent) {}
};

#endif
