#ifndef KARBONCALLIGRAPHYOPTIONWIDGET_H
#define KARBONCALLIGRAPHYOPTIONWIDGET_H

#include <QWidget>
#include <QMap>

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
    void loadProfile( const QString &name );

private:
    struct Profile
    {
        QString name;
        double width;
        double thinning;
        int angle;
        double fixation;
        double mass;
        double drag;
    };

    // convenience functions:

    // connects signals and slots
    void createConnections();

    // if they aren't already added adds the default profiles
    // called by the ctor
    void addDefaultProfiles();

    // laod the profiles from the configuration file
    void loadProfiles();

    // loads the profile set as current profile in the configuration file
    void loadCurrentProfile();

    // creates a profile reading the values from the input boxes
    // ownership of the Profile * object is passed to the caller
    Profile *createProfile( const QString &name );

private:
    QMap<QString, Profile *> profiles;

    KComboBox *comboBox;
    QDoubleSpinBox  *widthBox;
    QDoubleSpinBox  *thinningBox;
    QSpinBox        *angleBox;
    QDoubleSpinBox  *fixationBox;
    QDoubleSpinBox  *massBox;
    QDoubleSpinBox  *dragBox;
};

#endif // KARBONCALLIGRAPHYOPTIONWIDGET_H
