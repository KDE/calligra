/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#include "KoDiagrammAreaConfigDialog.h"

#include "KoDiagrammAreaConfigDialog.moc"

#include <kapp.h>
#include <klocale.h>

KoDiagrammAreaConfigDialog::KoDiagrammAreaConfigDialog(
													 KoDiagrammParameters* params,
													 QWidget* parent ) :
	KoDiagrammConfigDialog( params, parent  )
{
	// Fill pages with values
	defaults();

	setCaption( i18n( "Configuration of area charts" ) );
}
