/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMAREACONFIGDIALOG_H__
#define __KODIAGRAMMAREACONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;

class KoChartAreaConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartAreaConfigDialog( KoChartParameters* params, 
								QWidget*	parent );
};

#endif
