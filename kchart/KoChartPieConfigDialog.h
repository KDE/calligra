/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMPIECONFIGDIALOG_H__
#define __KODIAGRAMMPIECONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;

class KoChartPieConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartPieConfigDialog( KoChartParameters* params, 
								QWidget*	parent );
};

#endif
