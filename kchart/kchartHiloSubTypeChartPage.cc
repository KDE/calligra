/*
 *
 * Copyright 2001 by Laurent Montel <lmontel@mandrakesoft.com>
 */

#include "kchartHiloSubTypeChartPage.h"
#include "kchartHiloSubTypeChartPage.moc"

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

KChartHiloSubTypeChartPage::KChartHiloSubTypeChartPage( KDChartParams* params,
                                                        QWidget* parent ) :
    KChartSubTypeChartPage(  params, parent )
{
    QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
    QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
    toplevel->addWidget( subtypeBG, AlignCenter| AlignVCenter );
    normal = new QRadioButton( i18n( "Normal" ), subtypeBG );
    subtypeBG->insert( normal, KDChartParams::AreaNormal );
    stacked = new QRadioButton(i18n("HiLoClose"), subtypeBG );
    subtypeBG->insert( stacked, KDChartParams::AreaStacked );
    percent = new QRadioButton( i18n("HiLoOpenClose"), subtypeBG );
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

void KChartHiloSubTypeChartPage::init()
{
    switch( _params->hiLoChartSubType() ) {
    case KDChartParams::HiLoNormal:
        normal->setChecked( true );
        break;
    case KDChartParams::HiLoClose:
        stacked->setChecked( true );
        break;
    case KDChartParams::HiLoOpenClose:
        percent->setChecked( true );
        break;
    default:
        {
            kdDebug( 35001 ) << "Error in stack_type" << endl;
            abort();
            break;
        }
    }

    slotChangeSubType( _params->hiLoChartSubType() );
}

void KChartHiloSubTypeChartPage::slotChangeSubType( int type )
{
    switch( type ) {
    case KDChartParams::HiLoNormal:
        exampleLA->setPixmap( UserIcon( "areasubtypenormal", KChartFactory::global()  ) );
        break;
    case KDChartParams::HiLoClose:
        exampleLA->setPixmap( UserIcon( "areasubtypestacked", KChartFactory::global()  ) );
        break;
    case KDChartParams::HiLoOpenClose:
        exampleLA->setPixmap( UserIcon( "areasubtypepercent", KChartFactory::global()  ) );
        break;
    };
}



void KChartHiloSubTypeChartPage::apply()
{
    if( normal->isChecked() )
        _params->setHiLoChartSubType( KDChartParams::HiLoNormal );
    else if( stacked->isChecked() )
        _params->setHiLoChartSubType( KDChartParams::HiLoClose );
    else if( percent->isChecked() )
        _params->setHiLoChartSubType( KDChartParams::HiLoOpenClose );
    else {
        kdDebug( 35001 ) << "Error in groupbutton" << endl;
    }
}

