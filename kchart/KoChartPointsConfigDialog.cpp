/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoChartPointsConfigDialog.h"

#include "KoChartPointsConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoChartPointsConfigDialog::KoChartPointsConfigDialog(
														   KoChartParameters* params,
														   QWidget* parent ) :
	KoChartConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of points charts" ) );
}
