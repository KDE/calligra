/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammPieConfigDialog.h"

#include "KoDiagrammPieConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammPieConfigDialog::KoDiagrammPieConfigDialog(
													 KoDiagrammParameters* params,
													 QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of pie charts" ) );
}
