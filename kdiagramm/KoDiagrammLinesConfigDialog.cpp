/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammLinesConfigDialog.h"

#include "KoDiagrammLinesConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammLinesConfigDialog::KoDiagrammLinesConfigDialog(
													 KoDiagrammParameters* params,
													 QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of line charts" ) );
}
