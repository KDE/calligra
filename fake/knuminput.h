#ifndef FAKE_KNUMINPUT_H
#define FAKE_KNUMINPUT_H

#include <QSpinBox>

#include <QGridLayout>
#include <klocale.h>

class KSpinBox : public QSpinBox
{
public:
    KSpinBox(QWidget *parent = 0) : QSpinBox(parent) {}
    KSpinBox(int value, QWidget *parent) : QSpinBox(parent) { setValue(value); }
    void setRange(int min, int max, int step) { setMinimum(min); setMaximum(max); setSingleStep(step); }
};

class KIntSpinBox : public KSpinBox
{
public:
    KIntSpinBox(QWidget *parent = 0) : KSpinBox(parent) {}
    KIntSpinBox(int value, QWidget *parent) : KSpinBox(parent) { setValue(value); }
};

class KNumInput : public KSpinBox
{
public:
    KNumInput(QWidget *parent = 0) : KSpinBox(parent) {}
    KNumInput(int value, QWidget *parent) : KSpinBox(parent) { setValue(value); }
};

class KIntNumInput : public KNumInput
{
public:
    KIntNumInput(QWidget *parent = 0) : KNumInput(parent) {}
    KIntNumInput(int value, QWidget *parent) : KNumInput(parent) { setValue(value); }
};

class KDoubleNumInput : public KNumInput
{
public:
    KDoubleNumInput(QWidget *parent = 0) : KNumInput(parent) {}
    KDoubleNumInput(double lower, double upper, double value, QWidget *parent=0,double singleStep=0.01,
                    int precision=2) : KNumInput(parent) {setValue(value);}
    virtual void setLabel(const QString & label, Qt::Alignment a = Qt::AlignLeft | Qt::AlignTop) {}


};


#endif
