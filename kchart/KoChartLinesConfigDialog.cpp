/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartLinesConfigDialog.h"

#include "KoChartLinesConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartLinesConfigDialog::KoChartLinesConfigDialog(
													 KoChartParameters* params,
													 QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of line charts" ) );
}
