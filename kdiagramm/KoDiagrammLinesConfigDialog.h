/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMLINESCONFIGDIALOG_H__
#define __KODIAGRAMMLINESCONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;

class KoDiagrammLinesConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammLinesConfigDialog( KoDiagrammParameters* params, 
								 QWidget*	parent );
};

#endif
