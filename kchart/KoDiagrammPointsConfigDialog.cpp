/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammPointsConfigDialog.h"

#include "KoDiagrammPointsConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammPointsConfigDialog::KoDiagrammPointsConfigDialog(
														   KoDiagrammParameters* params,
														   QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of points charts" ) );
}
