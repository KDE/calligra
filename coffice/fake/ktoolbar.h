#ifndef FAKE_KTOOLBAR_H
#define FAKE_KTOOLBAR_H

#include <QToolBar>

class KToolBar : public QToolBar
{
    Q_OBJECT
public:
    KToolBar(QWidget *parent = 0) : QToolBar(parent) {}
};

#endif
