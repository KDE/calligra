/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTCONFIGDIALOG_H__
#define __KCHARTCONFIGDIALOG_H__

#include <qtabdialog.h>

#include "kchartColorConfigPage.h"
#include "kchartParameterConfigPage.h"
#include "kchartParameter3dConfigPage.h"
#include "kchartParameterPieConfigPage.h"
//#include "kchartGeometryConfigPage.h"
#include "kchartFontConfigPage.h"
#include "kchartPieConfigPage.h"
#include "kchartparams.h"


class KChartConfigDialog : public QTabDialog
{
    Q_OBJECT

public:
    KChartConfigDialog( KChartParameters* params,
			QWidget*	parent );

protected:
    KChartParameters* _params;
    //KChartGeometryConfigPage* _geompage;
    KChartColorConfigPage* _colorpage;
    KChartParameterConfigPage*_parameterpage;
    KChartParameter3dConfigPage*_parameter3dpage;
    KChartParameterPieConfigPage*_parameterpiepage;
    KChartFontConfigPage*_parameterfontpage;
    KChartPieConfigPage*_piepage;
protected slots:
    virtual void apply();
    virtual void defaults();
};

#endif
