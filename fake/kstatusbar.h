#ifndef FAKE_KSTATUSBAR_H
#define FAKE_KSTATUSBAR_H

#include <QStatusBar>

class KStatusBar : public QStatusBar
{
public:
    KStatusBar(QWidget *parent = 0) : QStatusBar(parent) {}
};

#endif
