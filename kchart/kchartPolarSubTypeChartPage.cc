/*
 *
 * Copyright 2001 by Laurent Montel <lmontel@mandrakesoft.com>
 */

#include "kchartPolarSubTypeChartPage.h"
#include "kchartPolarSubTypeChartPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qhgroupbox.h>
#include <qlayout.h>
#include <stdlib.h>

#include "kdchart/KDChartParams.h"

#include "kchart_factory.h"

KChartPolarSubTypeChartPage::KChartPolarSubTypeChartPage( KDChartParams* params,
                                                        QWidget* parent ) :
    KChartSubTypeChartPage(  params, parent )
{
    QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
    QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
    toplevel->addWidget( subtypeBG, AlignCenter| AlignVCenter );
    normal = new QRadioButton( i18n( "Normal" ), subtypeBG );
    subtypeBG->insert( normal, KDChartParams::AreaNormal );
    stacked = new QRadioButton( i18n( "Stacked" ), subtypeBG );
    subtypeBG->insert( stacked, KDChartParams::AreaStacked );
    percent = new QRadioButton( i18n( "Percent" ), subtypeBG );
    subtypeBG->insert( percent, KDChartParams::AreaPercent );
    subtypeBG->setFixedWidth( subtypeBG->sizeHint().width() );
    connect( subtypeBG, SIGNAL( clicked( int ) ),
             this, SLOT( slotChangeSubType( int ) ) );

    QHGroupBox* exampleGB = new QHGroupBox( i18n( "Example" ), this );
    toplevel->addWidget( exampleGB, 2 );
    exampleLA = new QLabel( exampleGB );
    exampleLA->setAlignment( AlignCenter | AlignVCenter );
    // PENDING(kalle) Make image scale with available space once Qt 2.2 is out.
}

void KChartPolarSubTypeChartPage::init()
{
    switch( _params->polarChartSubType() ) {
    case KDChartParams::PolarNormal:
        normal->setChecked( true );
        break;
    case KDChartParams::PolarStacked:
        stacked->setChecked( true );
        break;
    case KDChartParams::PolarPercent:
        percent->setChecked( true );
        break;
    default:
        {
            kdDebug( 35001 ) << "Error in stack_type" << endl;
            abort();
            break;
        }
    }

    slotChangeSubType( _params->lineChartSubType() );
}

void KChartPolarSubTypeChartPage::slotChangeSubType( int type )
{
    switch( type ) {
    case KDChartParams::PolarNormal:
        exampleLA->setPixmap( UserIcon( "polarsubtypenormal", KChartFactory::global()  ) );
        break;
    case KDChartParams::PolarStacked:
        exampleLA->setPixmap( UserIcon( "polarsubtypestacked", KChartFactory::global()  ) );
        break;
    case KDChartParams::PolarPercent:
        exampleLA->setPixmap( UserIcon( "polarsubtypepercent", KChartFactory::global()  ) );
        break;
    };
}



void KChartPolarSubTypeChartPage::apply()
{
    if( normal->isChecked() )
        _params->setPolarChartSubType( KDChartParams::PolarNormal );
    else if( stacked->isChecked() )
        _params->setPolarChartSubType( KDChartParams::PolarStacked );
    else if( percent->isChecked() )
        _params->setPolarChartSubType( KDChartParams::PolarPercent );
    else {
        kdDebug( 35001 ) << "Error in groupbutton" << endl;
    }
}

