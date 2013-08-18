#ifndef FAKE_KSTATUSBAR_H
#define FAKE_KSTATUSBAR_H

#include <QStatusBar>

class QStatusBar : public QStatusBar
{
public:
    QStatusBar(QWidget *parent = 0) : QStatusBar(parent) {}
};

#endif
