/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMLINESPOINTSCONFIGDIALOG_H__
#define __KODIAGRAMMLINESPOINTSCONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;

class KoDiagrammLinesPointsConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammLinesPointsConfigDialog( KoDiagrammParameters* params, 
									   QWidget*	parent );
};

#endif
