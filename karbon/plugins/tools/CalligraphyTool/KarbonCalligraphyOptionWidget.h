#ifndef KARBONCALLIGRAPHYOPTIONWIDGET_H
#define KARBONCALLIGRAPHYOPTIONWIDGET_H

#include <QWidget>

class KComboBox;
class QSpinBox;
class QDoubleSpinBox;

class KarbonCalligraphyOptionWidget : public QWidget
{
    Q_OBJECT
public:
    KarbonCalligraphyOptionWidget();
    ~KarbonCalligraphyOptionWidget();

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

private slots:
    void loadProfile( int profileIndex );

private:
    struct CalligraphyProfile
    {
        CalligraphyProfile( const QString &name, double width,
                            double thinning, int angle,
                            double fixation, double mass, double drag );
        QString name;
        double width;
        double thinning;
        int angle;
        double fixation;
        double mass;
        double drag;
    };

    void addDefaultProfiles();
    // laod the profiles from the configuration file
    void loadProfiles();
    void loadCurrentProfile();

    QList<CalligraphyProfile *>profiles;
    //int currentProfile; // index of current profile

    KComboBox *comboBox;
    QDoubleSpinBox  *widthBox;
    QDoubleSpinBox  *thinningBox;
    QSpinBox        *angleBox;
    QDoubleSpinBox  *fixationBox;
    QDoubleSpinBox  *massBox;
    QDoubleSpinBox  *dragBox;
};

#endif // KARBONCALLIGRAPHYOPTIONWIDGET_H
