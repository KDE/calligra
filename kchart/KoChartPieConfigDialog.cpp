/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartPieConfigDialog.h"

#include "KoChartPieConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartPieConfigDialog::KoChartPieConfigDialog(
													 KoChartParameters* params,
													 QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of pie charts" ) );
}
