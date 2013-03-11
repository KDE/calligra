#ifndef FAKE_KNUMINPUT_H
#define FAKE_KNUMINPUT_H

#include <QSpinBox>

#include <QGridLayout>
#include <klocale.h>

class KNumInput : public QSpinBox
{
public:
    KNumInput(QWidget *parent = 0) : QSpinBox(parent) {}
};

class KIntNumInput : public KNumInput
{
public:
    KIntNumInput(QWidget *parent = 0) : KNumInput(parent) {}
    KIntNumInput(int value, QWidget *parent) : KNumInput(parent) { setValue(value); }
    void setRange(int min, int max, int step) { setMinimum(min); setMaximum(max); setSingleStep(step); }
};

#endif
