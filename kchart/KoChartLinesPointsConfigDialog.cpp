/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartLinesPointsConfigDialog.h"

#include "KoChartLinesPointsConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartLinesPointsConfigDialog::KoChartLinesPointsConfigDialog(
															   KoChartParameters* params,
															   QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of lines/points charts" ) );
}
