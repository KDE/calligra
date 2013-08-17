#ifndef FAKE_KSELECTOR_H
#define FAKE_KSELECTOR_H

#include <QAbstractSlider>

class KSelector : public QAbstractSlider
{
public:
    KSelector(QWidget *parent = 0) : QAbstractSlider(parent) {}
    KSelector(Qt::Orientation o, QWidget *parent = 0) : QAbstractSlider(parent) { setOrientation(o); }
};

#endif
