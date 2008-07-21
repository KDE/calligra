#ifndef KARBONCALLIGRAPHYOPTIONWIDGET_H
#define KARBONCALLIGRAPHYOPTIONWIDGET_H

#include <QWidget>
#include <QMap>

class KComboBox;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;

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
    void usePathChanged(bool);
    void usePressureChanged(bool);
    void useAngleChanged(bool);
    void widthChanged(double);
    void thinningChanged(double);
    void angleChanged(int);
    void fixationChanged(double);
    void massChanged(double);
    void dragChanged(double);

private slots:
    void loadProfile( const QString &name );
    void toggleUsePressure( bool checked );
    void toggleUseAngle( bool checked );
    void updateCurrentProfile();
    void saveProfileAs();
    void removeProfile();
    void toggleDetails();

private:
    struct Profile
    {
        QString name;
        int index; // index in the config file
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

    // save a new profile using the values of the input boxes
    // if a profile with the same name already exists it will be overwritten
    void saveProfile( const QString &name );

    // removes the profile from the configuration file, from profiles
    // and from the combobox.
    // if the profile doesn't exist the function does nothing
    void removeProfile(const QString &name);

    // returns the position inside profiles of a certain profile
    // returns -1 if the profile is not found
    int profilePosition( const QString &profileName );

private:
    QMap<QString, Profile *> profiles;

    KComboBox *comboBox;
    QCheckBox *usePath;
    QCheckBox *usePressure;
    QCheckBox *useAngle;
    QDoubleSpinBox  *widthBox;
    QDoubleSpinBox  *thinningBox;
    QSpinBox        *angleBox;
    QDoubleSpinBox  *capsBox;
    QDoubleSpinBox  *fixationBox;
    QDoubleSpinBox  *massBox;
    QDoubleSpinBox  *dragBox;

    QPushButton *detailsButton;
    QPushButton *saveButton;
    QPushButton *removeButton;

    QWidget *details;

    // when true updateCurrentProfile() doesn't do anything
    bool changingProfile;

    bool detailsShowed;
};

#endif // KARBONCALLIGRAPHYOPTIONWIDGET_H
