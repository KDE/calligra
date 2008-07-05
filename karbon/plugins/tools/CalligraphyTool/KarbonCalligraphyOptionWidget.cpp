#include "KarbonCalligraphyOptionWidget.h"

#include <KLocale>
#include <KComboBox>
#include <KGlobal>
#include <KConfigGroup>
#include <KDebug>

#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

/*
Profiles are saved in karboncalligraphyrc

In the group "General", profile is the number of profile used

Every profile is described in a group, the name of which is "ProfileN",
where N is the number of the profile.

Default profiles are added by the function addDefaultProfiles(), once they
have been added, the entry defaultProfilesAdded in the "General" group is
set to true
TODO: add a reset defaults option?
*/

/*class CalligraphyProfiles
{
    void addProfile(const QString &name);
    void removeProfile(const QString &name);

    void selectProfile(const QString &name);
    QString selectedProfile();

private:
    loadAll();
    saveAll();

private:
    QString m_selectedProfile;
    QList<CalligraphyProfile> m_profiles;
};*/

KarbonCalligraphyOptionWidget::CalligraphyProfile::CalligraphyProfile(
                const QString &name, double width, double thinning, int angle,
                double fixation, double mass, double drag )
    : name(name), width(width), thinning(thinning), angle(angle),
      fixation(fixation), mass(mass), drag(drag)
{}

KarbonCalligraphyOptionWidget::KarbonCalligraphyOptionWidget()
{
    QVBoxLayout *layout = new QVBoxLayout( this );

    QLabel *profileLabel = new QLabel( i18n("Profile:"), this );
    layout->addWidget( profileLabel );
    comboBox = new KComboBox( this );
    layout->addWidget( comboBox );

    QHBoxLayout *widthLayout = new QHBoxLayout( this );
    QLabel *widthLabel = new QLabel( i18n( "Width" ), this );
    widthBox = new QDoubleSpinBox;
    widthBox->setRange( 0.0, 1000.0 );
    widthLayout->addWidget( widthLabel );
    widthLayout->addWidget( widthBox );
    layout->addLayout( widthLayout );

    QPushButton *detailsButton =
            new QPushButton( i18n("Hide details <<"), this );
    layout->addWidget( detailsButton );

    QHBoxLayout *thinningLayout = new QHBoxLayout( this );
    QLabel *thinningLabel = new QLabel( i18n( "Thinning" ), this );
    thinningBox = new QDoubleSpinBox;
    thinningBox->setRange( -1.0, 1.0 );
    thinningBox->setSingleStep( 0.1 );
    thinningLayout->addWidget( thinningLabel );
    thinningLayout->addWidget( thinningBox );
    layout->addLayout( thinningLayout );

    QHBoxLayout *angleLayout = new QHBoxLayout( this );
    QLabel *angleLabel = new QLabel( i18n( "Angle" ), this );
    angleBox = new QSpinBox;
    angleBox->setRange( 0, 180 );
    angleLayout->addWidget( angleLabel );
    angleLayout->addWidget( angleBox );
    layout->addLayout( angleLayout );

    QHBoxLayout *fixationLayout = new QHBoxLayout( this );
    QLabel *fixationLabel = new QLabel( i18n( "Fixation" ), this );
    fixationBox = new QDoubleSpinBox;
    fixationBox->setRange( 0.0, 1.0 );
    fixationBox->setSingleStep( 0.1 );
    fixationLayout->addWidget( fixationLabel );
    fixationLayout->addWidget( fixationBox );
    layout->addLayout( fixationLayout );

    QHBoxLayout *massLayout = new QHBoxLayout( this );
    QLabel *massLabel = new QLabel( i18n( "Mass" ), this );
    massBox = new QDoubleSpinBox;
    massBox->setRange( 0.0, 20.0 );
    massBox->setDecimals( 1 );
    massLayout->addWidget( massLabel );
    massLayout->addWidget( massBox );
    layout->addLayout( massLayout );

    QHBoxLayout *dragLayout = new QHBoxLayout( this );
    QLabel *dragLabel = new QLabel( i18n( "Drag" ), this );
    dragBox = new QDoubleSpinBox;
    dragBox->setRange( 0.0, 1.0 );
    dragBox->setSingleStep( 0.1 );
    dragLayout->addWidget( dragLabel );
    dragLayout->addWidget( dragBox );
    layout->addLayout( dragLayout );

    QPushButton *saveButton =
            new QPushButton( i18n("Save profile as..."), this );
    layout->addWidget( saveButton );

    QPushButton *removeButton =
            new QPushButton( i18n("Remove profile"), this );
    layout->addWidget( removeButton );
    layout->addStretch( 1 );

    connect ( comboBox, SIGNAL(currentIndexChanged(int)),
              SLOT(loadProfile(int)) );

    connect( widthBox, SIGNAL(valueChanged(double)),
             SIGNAL(widthChanged(double)));

    connect( thinningBox, SIGNAL(valueChanged(double)),
             SIGNAL(thinningChanged(double)));

    connect( angleBox, SIGNAL(valueChanged(int)),
             SIGNAL(angleChanged(int)));

    connect( fixationBox, SIGNAL(valueChanged(double)),
             SIGNAL(fixationChanged(double)));

    connect( massBox, SIGNAL(valueChanged(double)),
             SIGNAL(massChanged(double)));

    connect( dragBox, SIGNAL(valueChanged(double)),
             SIGNAL(dragChanged(double)));

    addDefaultProfiles();
    loadProfiles();
}

KarbonCalligraphyOptionWidget::~KarbonCalligraphyOptionWidget()
{
    while ( ! profiles.isEmpty() )
        delete profiles.takeLast();
}

void KarbonCalligraphyOptionWidget::addDefaultProfiles()
{
    // check if the profiles where already added
    KConfig config( KGlobal::mainComponent(), "karboncalligraphyrc" );
    KConfigGroup generalGroup( &config, "General" );

    if ( generalGroup.readEntry( "defaultProfilesAdded", false ) )
        return;

    // if not add them
    KConfigGroup profile0( &config, "Profile0" );
    profile0.writeEntry( "name", i18n("Mouse") );
    profile0.writeEntry( "width", 30.0 );
    profile0.writeEntry( "thinning", 0.2 );
    profile0.writeEntry( "angle", 30 );
    profile0.writeEntry( "fixation", 0.0 );
    profile0.writeEntry( "mass", 3.0 );
    profile0.writeEntry( "drag", 0.7 );
    
    KConfigGroup profile1( &config, "Profile1" );
    profile1.writeEntry( "name", i18n("Graphics Pen") );
    profile1.writeEntry( "width", 50.0 );
    profile1.writeEntry( "thinning", 0.2 );
    profile1.writeEntry( "angle", 30 );
    profile1.writeEntry( "fixation", 0.0 );
    profile1.writeEntry( "mass", 1.0 );
    profile1.writeEntry( "drag", 0.9 );

    generalGroup.writeEntry( "profile", 0);
    generalGroup.writeEntry( "defaultProfilesAdded", true );

    config.sync();
}


void KarbonCalligraphyOptionWidget::loadProfiles()
{
    KConfig config( KGlobal::mainComponent(), "karboncalligraphyrc" );

    // load profiles as long as they are present
    int i = 0;
    while (1) // forever
    {
        KConfigGroup profile( &config, "Profile" + QString::number(i) );
        // invalid profile, assume we reached the last one
        if ( ! profile.hasKey("name") )
            break;

        QString name =      profile.readEntry( "name", QString() );
        double width =      profile.readEntry( "width", 30.0 );
        double thinning =   profile.readEntry( "thinning", 0.2 );
        int angle =         profile.readEntry( "angle", 30 );
        double fixation =   profile.readEntry( "fixation", 0.0 );
        double mass =       profile.readEntry( "mass", 3.0 );
        double drag =       profile.readEntry( "drag", 0.7 );

        profiles.append( new CalligraphyProfile( name, width, thinning,
                                                 angle, fixation,
                                                 mass, drag ) );
        comboBox->addItem( name );
        ++i;
    }

    loadCurrentProfile();
}

void KarbonCalligraphyOptionWidget::loadCurrentProfile()
{
    KConfig config( KGlobal::mainComponent(), "karboncalligraphyrc" );
    KConfigGroup generalGroup( &config, "General" );
    int currentProfile = generalGroup.readEntry( "profile", -1 );

    if ( currentProfile < 0 || currentProfile >= profiles.count() ) {
        kError() << "invalid karboncalligraphyrc!!";
        return;
    }

    comboBox->setCurrentIndex( currentProfile );

    CalligraphyProfile *profile = profiles[currentProfile];
    widthBox->setValue( profile->width );
    thinningBox->setValue( profile->thinning );
    angleBox->setValue( profile->angle );
    fixationBox->setValue( profile->fixation );
    massBox->setValue( profile->mass );
    dragBox->setValue( profile->drag );
}

void KarbonCalligraphyOptionWidget::loadProfile( int profileIndex )
{
    KConfig config( KGlobal::mainComponent(), "karboncalligraphyrc" );
    KConfigGroup generalGroup( &config, "General" );
    generalGroup.writeEntry( "profile", profileIndex );
    config.sync();

    loadCurrentProfile();
}

void KarbonCalligraphyOptionWidget::emitAll()
{
    emit widthChanged( widthBox->value() );
    emit thinningChanged( thinningBox->value() );
    emit angleChanged( angleBox->value() );
    emit fixationChanged( fixationBox->value() );
    emit massChanged( massBox->value() );
    emit dragChanged( dragBox->value() );
}
