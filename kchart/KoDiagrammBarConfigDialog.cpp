/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammBarConfigDialog.h"
#include "KoDiagrammBarConfigPage.h"

#include "KoDiagrammBarConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammBarConfigDialog::KoDiagrammBarConfigDialog(
													 KoDiagrammParameters* params,
													 QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Bar page
	_barpage = new KoDiagrammBarConfigPage( this );
	addTab( _barpage, i18n( "&Bars" ) );

	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of bar charts" ) );
}


void KoDiagrammBarConfigDialog::apply()
{
	KoDiagrammConfigDialog::apply();
	_params->_overwrite = _barpage->overwriteMode();
	_params->_xbardist = _barpage->xAxisDistance();
}


void KoDiagrammBarConfigDialog::defaults()
{
	KoDiagrammConfigDialog::defaults();
	_barpage->setOverwriteMode( _params->_overwrite );
	_barpage->setXAxisDistance( _params->_xbardist );
}



