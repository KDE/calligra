/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMBARCONFIGDIALOG_H__
#define __KODIAGRAMMBARCONFIGDIALOG_H__

#include "KoChartConfigDialog.h"

class KoChartParameters;
class KoChartBarConfigPage;

class KoChartBarConfigDialog : public KoChartConfigDialog
{
	Q_OBJECT

public:
	KoChartBarConfigDialog( KoChartParameters* params, 
							   QWidget*	parent );

public slots:
	virtual void apply();
	virtual void defaults();

private:
	KoChartBarConfigPage* _barpage;
};

#endif
