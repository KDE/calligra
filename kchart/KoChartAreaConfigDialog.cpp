/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartAreaConfigDialog.h"

#include "KoChartAreaConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartAreaConfigDialog::KoChartAreaConfigDialog(
			    KoChartParameters* params,
		    									 QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of area charts" ) );
}
