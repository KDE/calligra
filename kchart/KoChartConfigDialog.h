/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMCONFIGDIALOG_H__
#define __KODIAGRAMMCONFIGDIALOG_H__

#include <qtabdialog.h>

class KoChartParameters;
class KoChartColorConfigPage;
class KoChartGeometryConfigPage;

class KoChartConfigDialog : public QTabDialog
{
	Q_OBJECT

public:
	KoChartConfigDialog( KoChartParameters* params, 
							QWidget*	parent );

protected:
	KoChartParameters* _params;
	KoChartGeometryConfigPage* _geompage;
	KoChartColorConfigPage* _colorpage;

protected slots:
	virtual void apply();
	virtual void defaults();
};

#endif
