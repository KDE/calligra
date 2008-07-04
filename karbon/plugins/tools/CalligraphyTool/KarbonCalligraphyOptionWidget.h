#ifndef KARBONCALLIGRAPHYOPTIONWIDGET_H
#define KARBONCALLIGRAPHYOPTIONWIDGET_H

#include <QWidget>

class QSpinBox;
class QDoubleSpinBox;

class KarbonCalligraphyOptionWidget : public QWidget
{
    Q_OBJECT
public:
    KarbonCalligraphyOptionWidget();

    // emits all signals with the appropriate values
    // called once the signals are connected inside KarbonCalligraphyTool
    // to make sure all parameters are uptodate
    void emitAll();

signals:
    // all the following signals emit user friendly values, not the internal
    // values which are instead computed directly by KarbonCalligraphyTool
    void widthChanged(double);
    void thinningChanged(double);
    void angleChanged(int);
    void fixationChanged(double);
    void massChanged(double);
    void dragChanged(double);

private:
    QDoubleSpinBox  *widthBox;
    QDoubleSpinBox  *thinningBox;
    QSpinBox        *angleBox;
    QDoubleSpinBox  *fixationBox;
    QDoubleSpinBox  *massBox;
    QDoubleSpinBox  *dragBox;
};

#endif // KARBONCALLIGRAPHYOPTIONWIDGET_H
