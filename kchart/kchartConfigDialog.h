/*
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTCONFIGDIALOG_H__
#define __KCHARTCONFIGDIALOG_H__

#include <qtabdialog.h>
#include "kchartDataEditor.h"
class KDChartParams;
class KChartColorConfigPage;
class KChartParameterConfigPage;
class KChartParameter3dConfigPage;
class KChartParameterPieConfigPage;
class KChartFontConfigPage;
class KChartPieConfigPage;
class KChartSubTypeChartPage;
class KChartComboPage;
class KChartBackgroundPixmapConfigPage;
class KChartLegendConfigPage;

class KChartConfigDialog : public QTabDialog
{
    Q_OBJECT

public:
    enum { KC_FONT = 1, KC_COLORS = 2, KC_BACK = 4, KC_LEGEND=8 };
    KChartConfigDialog( KDChartParams* params,
                        QWidget* parent, int flags,KoChart::Data *dat );

    void setCurrentPage( int page );
signals:
	void dataChanged();

protected:
    KDChartParams* _params;
    //KChartGeometryConfigPage* _geompage;
    KChartColorConfigPage* _colorpage;
    KChartParameterConfigPage*_parameterpage;
    KChartParameter3dConfigPage*_parameter3dpage;
    KChartParameterPieConfigPage*_parameterpiepage;
    KChartFontConfigPage*_parameterfontpage;
    KChartPieConfigPage*_piepage;
    KChartSubTypeChartPage *_subTypePage;
    KChartBackgroundPixmapConfigPage* _backgroundpixpage;
    KChartComboPage *_hlcChart;
    KChartLegendConfigPage *_parameterLegend;
protected slots:
    virtual void apply();
    virtual void defaults();
};

#endif
