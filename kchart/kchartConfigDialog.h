/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTCONFIGDIALOG_H__
#define __KCHARTCONFIGDIALOG_H__

#include <qtabdialog.h>

class KChartParameters;
class KChartColorConfigPage;
class KChartGeometryConfigPage;

class KChartConfigDialog : public QTabDialog
{
    Q_OBJECT

public:
    KChartConfigDialog( KChartParameters* params, 
			QWidget*	parent );

protected:
    KChartParameters* _params;
    KChartGeometryConfigPage* _geompage;
    KChartColorConfigPage* _colorpage;

    protected slots:
	virtual void apply();
    virtual void defaults();
};

#endif
