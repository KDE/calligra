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
    qDebug( "Sorry, not implemented: KChartAreaSubTypeChartPage::KChartAreaSubTypeChartPage()" );
#warning Put back in
#ifdef K

    QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
  QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
  toplevel->addWidget( subtypeBG, AlignCenter| AlignVCenter );
  depth = new QRadioButton( i18n( "Depth" ), subtypeBG );
  subtypeBG->insert( depth, KCHARTSTACKTYPE_DEPTH );
  beside = new QRadioButton( i18n( "Beside" ), subtypeBG );
  subtypeBG->insert( beside, KCHARTSTACKTYPE_BESIDE );
  subtypeBG->setFixedWidth( subtypeBG->sizeHint().width() );
  connect( subtypeBG, SIGNAL( clicked( int ) ),
                   this, SLOT( slotChangeSubType( int ) ) );

  QHGroupBox* exampleGB = new QHGroupBox( i18n( "Example" ), this );
  toplevel->addWidget( exampleGB, 2 );
  exampleLA = new QLabel( exampleGB );
  exampleLA->setAlignment( AlignCenter | AlignVCenter );
  // PENDING(kalle) Make image scale with available space once Qt 2.2 is out.
#endif
}


void KChartAreaSubTypeChartPage::init()
{
    qDebug( "Sorry, not implemented: KChartAreaSubTypeChartPage::init()" );
#warning Put back in
#ifdef K
  // LAYER and PERCENT are for bars only and therefore not configurable here.
  switch((int)_params->stack_type) {
        case (int)KCHARTSTACKTYPE_DEPTH:
          {
                depth->setChecked(true);
                break;
          }
        case (int)KCHARTSTACKTYPE_BESIDE:
          {
                beside->setChecked(true);
                break;
          }
        default:
          {
                kdDebug( 35001 ) << "Error in stack_type" << endl;
                abort();
                break;
          }
        }

  slotChangeSubType( _params->stack_type );
#endif
}

void KChartAreaSubTypeChartPage::slotChangeSubType( int type )
{
    qDebug( "Sorry, not implemented: KChartAreaSubTypeChartPage::slotChangeSubType()" );
#warning Put back in
#ifdef K
  switch( type ) {
  case KCHARTSTACKTYPE_DEPTH:
        exampleLA->setPixmap( UserIcon( "areasubtypedepth" ) );
        break;
  case KCHARTSTACKTYPE_BESIDE:
        exampleLA->setPixmap( UserIcon( "areasubtypebeside" ) );
        break;
  };
#endif
}



void KChartAreaSubTypeChartPage::apply()
{
    qDebug( "Sorry, not implemented: KChartAreaSubTypeChartPage::apply()" );
#warning Put back in
#ifdef K
  if( depth->isChecked() ) {
        _params->stack_type = KCHARTSTACKTYPE_DEPTH;
  } else if( beside->isChecked() ) {
        _params->stack_type = KCHARTSTACKTYPE_BESIDE;
  } else {
        kdDebug( 35001 ) << "Error in groupbutton" << endl;
  }
#endif
}

