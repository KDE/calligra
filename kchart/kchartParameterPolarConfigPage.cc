/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameterPolarConfigPage.h"

#include "kchartParameterPolarConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include "kchart_params.h"

KChartParameterPolarConfigPage::KChartParameterPolarConfigPage( KChartParams* params,
                                                            QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QGridLayout *grid1 = new QGridLayout(this,2,2,15,7);
    QGroupBox* gb3 = new QGroupBox( i18n( "Parameter" ), this );
    QGridLayout *grid3 = new QGridLayout(gb3,8,2,15,7);

    polarMarker=new QCheckBox(i18n("Polar marker"),gb3);
    grid3->addWidget(polarMarker,2,0);

    showCircularLabel=new QCheckBox(i18n("Show circular label"),gb3);
    grid3->addWidget(showCircularLabel,3,0);

    QLabel *label = new QLabel( i18n( "Zero degree position:" ), gb3 );
    label->resize( label->sizeHint() );
    grid3->addWidget( label,4,0);

    angle = new QSpinBox(-359, 359, 1, gb3);
    angle->resize(100, angle->sizeHint().height() );
    grid3->addWidget( angle,5,0);

    label=new QLabel(i18n("Line width:"),gb3);
    label->resize(label->sizeHint());
    grid3->addWidget(label,6,0);

    lineWidth=new QSpinBox(gb3);
    grid3->addWidget(lineWidth,7,0);

    grid1->addWidget(gb3,0,0);
}


void KChartParameterPolarConfigPage::init()
{
    polarMarker->setChecked(_params->polarMarker());
    angle->setValue( _params->polarZeroDegreePos() );
    showCircularLabel->setChecked(_params->polarRotateCircularLabels());
    lineWidth->setValue(_params->polarLineWidth());
}


void KChartParameterPolarConfigPage::apply()
{
    _params->setPolarZeroDegreePos(angle->value());
    _params->setPolarMarker(polarMarker->isChecked());
    _params->setPolarRotateCircularLabels(showCircularLabel->isChecked());
    _params->setPolarLineWidth(lineWidth->value());
}
