/*
 * $Id$
 *
 * Copyright 1999-2000 by Matthias Kalle Dalheimer, <kalle@dalheimer.de>
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
#include "kchartparams.h"

KChartBarSubTypeChartPage::KChartBarSubTypeChartPage( KChartParameters* params,
													  QWidget* parent ) :
  KChartSubTypeChartPage( params, parent )
{
  QHBoxLayout* toplevel = new QHBoxLayout( this, 10 );
  QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
  toplevel->addWidget( subtypeBG, AlignCenter );
  depth = new QRadioButton( i18n( "Depth" ), subtypeBG ); ;
  subtypeBG->insert( depth, KCHARTSTACKTYPE_DEPTH );
  layer = new QRadioButton( i18n( "Layer" ), subtypeBG );
  subtypeBG->insert( layer, KCHARTSTACKTYPE_LAYER );
  beside = new QRadioButton( i18n( "Beside" ), subtypeBG );
  subtypeBG->insert( beside, KCHARTSTACKTYPE_BESIDE );
  percent = new QRadioButton( i18n( "Percent" ), subtypeBG );
  subtypeBG->insert( percent, KCHARTSTACKTYPE_PERCENT );
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
  switch((int)_params->stack_type) {
	case (int)KCHARTSTACKTYPE_DEPTH:
	  {
		depth->setChecked(true);
		break;
	  }
	case (int)KCHARTSTACKTYPE_LAYER:
	  {
		layer->setChecked(true);
		break;
	  }
	case (int)KCHARTSTACKTYPE_BESIDE:
	  {
		beside->setChecked(true);
		break;
	  }
	case (int)KCHARTSTACKTYPE_PERCENT:
	  {
		percent->setChecked(true);
		break;
	  }
	default:
	  {
		kdDebug( 35001 ) << "Error in stack_type" << endl;
		break;
	  }
	}
  
  slotChangeSubType( _params->stack_type );
}


void KChartBarSubTypeChartPage::slotChangeSubType( int type )
{
  switch( type ) {
  case KCHARTSTACKTYPE_DEPTH:
	exampleLA->setPixmap( UserIcon( "barsubtypedepth", KChartFactory::global()) );
	break;
  case KCHARTSTACKTYPE_LAYER:
	exampleLA->setPixmap( UserIcon( "barsubtypelayer", KChartFactory::global() ) );
	break;
  case KCHARTSTACKTYPE_BESIDE:
	exampleLA->setPixmap( UserIcon( "barsubtypebeside", KChartFactory::global() ) );
	break;
  case KCHARTSTACKTYPE_PERCENT:
	exampleLA->setPixmap( UserIcon( "barsubtypepercent", KChartFactory::global() ) );
	break;
  };
}

void KChartBarSubTypeChartPage::apply()
{
  if( depth->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_DEPTH;
  } else if( layer->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_LAYER;
  } else if( beside->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_BESIDE;
  } else if( percent->isChecked() )	{
	_params->stack_type = KCHARTSTACKTYPE_PERCENT;
  } else{
	kdDebug( 35001 ) << "Error in groupbutton" << endl;
  }
}

