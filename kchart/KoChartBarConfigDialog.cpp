/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartBarConfigDialog.h"
#include "KoChartBarConfigPage.h"

#include "KoChartBarConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartBarConfigDialog::KoChartBarConfigDialog(
													 KoChartParameters* params,
													 QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Bar page
	_barpage = new KoChartBarConfigPage( this );
	addTab( _barpage, i18n( "&Bars" ) );

	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of bar charts" ) );
}


void KoChartBarConfigDialog::apply()
{
	KoChartConfigDialog::apply();
	_params->_overwrite = _barpage->overwriteMode();
	_params->_xbardist = _barpage->xAxisDistance();
}


void KoChartBarConfigDialog::defaults()
{
	KoChartConfigDialog::defaults();
	_barpage->setOverwriteMode( _params->_overwrite );
	_barpage->setXAxisDistance( _params->_xbardist );
}



