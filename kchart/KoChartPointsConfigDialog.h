/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMPOINTSCONFIGDIALOG_H__
#define __KODIAGRAMMPOINTSCONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;

class KoChartPointsConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartPointsConfigDialog( KoChartParameters* params, 
								  QWidget*	parent );
};

#endif
