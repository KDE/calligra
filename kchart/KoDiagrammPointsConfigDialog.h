/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMPOINTSCONFIGDIALOG_H__
#define __KODIAGRAMMPOINTSCONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;

class KoDiagrammPointsConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammPointsConfigDialog( KoDiagrammParameters* params, 
								  QWidget*	parent );
};

#endif
