/*
 * $Id$
 *
 * Copyright 1999-2000 by Matthias Kalle Dalheimer, <kalle@dalheimer.de>
 */

#include "kchartAreaSubTypeChartPage.h"
#include "kchartAreaSubTypeChartPage.moc"

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

KChartAreaSubTypeChartPage::KChartAreaSubTypeChartPage( KDChartParams* params,
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


void KChartAreaSubTypeChartPage::init()
{
    switch( _params->areaChartSubType() ) {
    case KDChartParams::AreaNormal:
        normal->setChecked( true );
        break;
    case KDChartParams::AreaStacked:
        stacked->setChecked( true );
        break;
    case KDChartParams::AreaPercent:
        percent->setChecked( true );
        break;
    default:
        {
            kdDebug( 35001 ) << "Error in stack_type" << endl;
            abort();
            break;
        }
    }

    slotChangeSubType( _params->areaChartSubType() );
}

void KChartAreaSubTypeChartPage::slotChangeSubType( int type )
{
    switch( type ) {
    case KDChartParams::AreaNormal:
        exampleLA->setPixmap( UserIcon( "areasubtypenormal" ) );
        break;
    case KDChartParams::AreaStacked:
        exampleLA->setPixmap( UserIcon( "areasubtypestacked" ) );
        break;
    case KDChartParams::AreaPercent:
        exampleLA->setPixmap( UserIcon( "areasubtypepercent" ) );
        break;
    };
}



void KChartAreaSubTypeChartPage::apply()
{
    if( normal->isChecked() ) 
        _params->setAreaChartSubType( KDChartParams::AreaNormal );
    else if( stacked->isChecked() )
        _params->setAreaChartSubType( KDChartParams::AreaStacked );
    else if( percent->isChecked() )
        _params->setAreaChartSubType( KDChartParams::AreaPercent );
    else {
        kdDebug( 35001 ) << "Error in groupbutton" << endl;
    }
}

