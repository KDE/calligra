/*
 * $Id$
 *
 * Copyright 1999-2000 by Matthias Kalle Dalheimer, <kalle@dalheimer.de>
 */

#include "kchartAreaSubTypeChartPage.h"
#include "kchartAreaSubTypeChartPage.moc"

#include "kchartparams.h"

#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>

KChartAreaSubTypeChartPage::KChartAreaSubTypeChartPage(KChartParameters* params,QWidget* parent ) :
  KChartSubTypeChartPage(  params, parent )
{
  QVButtonGroup* subtypeBG = new QVButtonGroup( i18n( "Subtype" ), this );
  depth = new QRadioButton( i18n( "Depth" ), subtypeBG ); ;
  beside = new QRadioButton( i18n( "Beside" ), subtypeBG );
  sum = new QRadioButton( i18n( "Sum" ), subtypeBG );
}

void KChartAreaSubTypeChartPage::init()
{
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
	case (int)KCHARTSTACKTYPE_SUM:
	  {
		sum->setChecked(true);
		break;
	  }
	default:
	  {
		kdDebug( 35001 ) << "Error in stack_type" << endl;
		break;
	  }
	}
}

void KChartAreaSubTypeChartPage::apply()
{
  if( depth->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_DEPTH;
  } else if( beside->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_BESIDE;
  } else if( sum->isChecked() ) {
	_params->stack_type = KCHARTSTACKTYPE_SUM;
  } else {
	kdDebug( 35001 ) << "Error in groupbutton" << endl;
  }
}

