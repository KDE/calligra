/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammLinesPointsConfigDialog.h"

#include "KoDiagrammLinesPointsConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammLinesPointsConfigDialog::KoDiagrammLinesPointsConfigDialog(
															   KoDiagrammParameters* params,
															   QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of lines/points charts" ) );
}
