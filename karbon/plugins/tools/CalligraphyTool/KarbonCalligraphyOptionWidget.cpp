#include "KarbonCalligraphyOptionWidget.h"

#include <knuminput.h>
#include <klocale.h>
#include <kcombobox.h>

#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>



/*struct CalligraphyProfile
{
    QString name;
    double width;
};

class CalligraphyProfiles
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

KarbonCalligraphyOptionWidget::KarbonCalligraphyOptionWidget()
{
    QVBoxLayout *layout = new QVBoxLayout( this );

    QLabel *profileLabel = new QLabel( i18n("Profile:"), this );
    layout->addWidget( profileLabel );
    KComboBox *comboBox = new KComboBox( this );
    layout->addWidget( comboBox );

    QHBoxLayout *widthLayout = new QHBoxLayout( this );
    QLabel *widthLabel = new QLabel( i18n( "Width" ), this );
    widthBox = new QDoubleSpinBox;
    widthBox->setRange( 0.0, 1000.0 );
    widthBox->setValue( 50.0 );
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
    thinningBox->setValue( 0.0 );
    thinningLayout->addWidget( thinningLabel );
    thinningLayout->addWidget( thinningBox );
    layout->addLayout( thinningLayout );

    QHBoxLayout *angleLayout = new QHBoxLayout( this );
    QLabel *angleLabel = new QLabel( i18n( "Angle" ), this );
    angleBox = new QSpinBox;
    angleBox->setRange( 0, 180 );
    angleBox->setValue( 30 );
    angleLayout->addWidget( angleLabel );
    angleLayout->addWidget( angleBox );
    layout->addLayout( angleLayout );

    QHBoxLayout *fixationLayout = new QHBoxLayout( this );
    QLabel *fixationLabel = new QLabel( i18n( "Fixation" ), this );
    fixationBox = new QDoubleSpinBox;
    fixationBox->setRange( 0.0, 1.0 );
    fixationBox->setSingleStep( 0.1 );
    fixationBox->setValue( 1.0 );
    fixationLayout->addWidget( fixationLabel );
    fixationLayout->addWidget( fixationBox );
    layout->addLayout( fixationLayout );

    QHBoxLayout *massLayout = new QHBoxLayout( this );
    QLabel *massLabel = new QLabel( i18n( "Mass" ), this );
    massBox = new QDoubleSpinBox;
    massBox->setRange( 0.0, 20.0 );
    massBox->setDecimals( 1 );
    massBox->setValue( 4 );
    massLayout->addWidget( massLabel );
    massLayout->addWidget( massBox );
    layout->addLayout( massLayout );

    QHBoxLayout *dragLayout = new QHBoxLayout( this );
    QLabel *dragLabel = new QLabel( i18n( "Drag" ), this );
    dragBox = new QDoubleSpinBox;
    dragBox->setRange( 0.0, 1.0 );
    dragBox->setSingleStep( 0.1 );
    dragBox->setValue( 1.0 );
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
}


void KarbonCalligraphyOptionWidget::emitAll()
{
    emit widthChanged(widthBox->value());
    emit thinningChanged(thinningBox->value());
    emit angleChanged(angleBox->value());
    emit fixationChanged(fixationBox->value());
    emit massChanged(massBox->value());
    emit dragChanged(dragBox->value());
}
