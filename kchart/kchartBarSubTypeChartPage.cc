/*
 * $Id$
 *
 * Copyright 1999-2001 by Matthias Kalle Dalheimer, <kalle@dalheimer.de>
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartBarSubTypeChartPage.h"
#include "kchartBarSubTypeChartPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qhgroupbox.h>

#include "kchart_factory.h"

#include "kdchart/KDChartParams.h"

KChartBarSubTypeChartPage::KChartBarSubTypeChartPage( KDChartParams* params,
                                                      QWidget* parent ) :
    KChartSubTypeChartPage( params, parent )
{
    QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
    QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
    toplevel->addWidget( subtypeBG, AlignCenter );
    layer = new QRadioButton( i18n( "Layer" ), subtypeBG );
    subtypeBG->insert( layer, KDChartParams::BarStacked );
    beside = new QRadioButton( i18n( "Beside" ), subtypeBG );
    subtypeBG->insert( beside, KDChartParams::BarNormal );
    percent = new QRadioButton( i18n( "Percent" ), subtypeBG );
    subtypeBG->insert( percent, KDChartParams::BarPercent );
    subtypeBG->setFixedWidth( subtypeBG->sizeHint().width() );
    connect( subtypeBG, SIGNAL( clicked( int ) ),
             this, SLOT( slotChangeSubType( int ) ) );

    QHGroupBox* exampleGB = new QHGroupBox( i18n( "Example" ), this );
    toplevel->addWidget( exampleGB, 2 );
    exampleLA = new QLabel( exampleGB );
    exampleLA->setAlignment( AlignCenter | AlignVCenter );
    // PENDING(kalle) Make image scale with available space once Qt 2.2 is out.
}

void KChartBarSubTypeChartPage::init()
{
    // SUM is for areas only and therefore not configurable here.
    switch( _params->barChartSubType() ) {
    case KDChartParams::BarNormal:
        beside->setChecked( true );
        break;
    case KDChartParams::BarStacked:
        layer->setChecked( true );
        break;
    case KDChartParams::BarPercent:
        percent->setChecked( true );
        break;
    default:
        {
            kdDebug( 35001 ) << "Error in stack_type" << endl;
            break;
        }
    }

    slotChangeSubType( _params->barChartSubType() );
}


void KChartBarSubTypeChartPage::slotChangeSubType( int type )
{
    switch( type ) {
    case KDChartParams::BarStacked:
	exampleLA->setPixmap( UserIcon( "barsubtypelayer", KChartFactory::global() ) );
	break;
    case KDChartParams::BarNormal:
	exampleLA->setPixmap( UserIcon( "barsubtypebeside", KChartFactory::global() ) );
	break;
    case KDChartParams::BarPercent:
	exampleLA->setPixmap( UserIcon( "barsubtypepercent", KChartFactory::global() ) );
	break;
    };
}


void KChartBarSubTypeChartPage::apply()
{
    if( layer->isChecked() ) {
        _params->setBarChartSubType( KDChartParams::BarStacked );
    } else if( beside->isChecked() ) {
        _params->setBarChartSubType( KDChartParams::BarNormal );
    } else if( percent->isChecked() )	{
        _params->setBarChartSubType( KDChartParams::BarPercent );
    } else {
        kdDebug( 35001 ) << "Error in groupbutton" << endl;
    }
}

