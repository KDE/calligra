/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMPIECONFIGDIALOG_H__
#define __KODIAGRAMMPIECONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;

class KoDiagrammPieConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammPieConfigDialog( KoDiagrammParameters* params, 
								QWidget*	parent );
};

#endif
