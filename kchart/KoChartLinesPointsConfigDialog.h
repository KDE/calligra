/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMLINESPOINTSCONFIGDIALOG_H__
#define __KODIAGRAMMLINESPOINTSCONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;

class KoChartLinesPointsConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartLinesPointsConfigDialog( KoChartParameters* params, 
									   QWidget*	parent );
};

#endif
