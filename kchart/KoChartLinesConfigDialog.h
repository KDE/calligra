/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMLINESCONFIGDIALOG_H__
#define __KODIAGRAMMLINESCONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;

class KoChartLinesConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartLinesConfigDialog( KoChartParameters* params, 
								 QWidget*	parent );
};

#endif
