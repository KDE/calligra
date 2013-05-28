#ifndef FAKE_KNUMINPUT_H
#define FAKE_KNUMINPUT_H

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <klocale.h>

class KSpinBox : public QSpinBox
{
public:
    KSpinBox(QWidget *parent = 0) : QSpinBox(parent) {}
    KSpinBox(int value, QWidget *parent) : QSpinBox(parent) { setValue(value); }
    QString label() const { return QString(); }
    void setLabel(const QString &label) {}
    void setRange(int min, int max, int step) { setMinimum(min); setMaximum(max); setSingleStep(step); }
    bool sliderEnabled() const { return true; }
    void setSliderEnabled(bool) {}
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
    KIntNumInput(int value, QWidget *parent=0, int base = 10) : KNumInput(value, parent) {}
};

class KDoubleNumInput : public QDoubleSpinBox
{
public:
    KDoubleNumInput(QWidget *parent = 0) : QDoubleSpinBox(parent) {}
    KDoubleNumInput(double lower, double upper, double value, QWidget *parent=0, double singleStep=0.01, int precision=2) : QDoubleSpinBox(parent) {
        setRange(lower, upper);
        setSingleStep(singleStep);
        if (precision >= 0)
            setDecimals(precision);
        setValue(value);
    }
};

#endif
