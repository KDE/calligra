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
class KChartHeaderFooterConfigPage;
class KChartLegendConfigPage;

class KChartConfigDialog : public QTabDialog
{
    Q_OBJECT

public:
    enum { KC_FONT = 1, KC_COLORS = 2, KC_BACK = 4, KC_LEGEND=8, KC_SUBTYPE=16,KC_HEADERFOOTER=32,KC_ALL=256 };
    KChartConfigDialog( KChartParams* params,
                        QWidget* parent, int flags,KoChart::Data *dat );

    void init3dPage();
signals:
	void dataChanged();

protected:
    KChartParams* _params;
    //KChartGeometryConfigPage* _geompage;
    KChartColorConfigPage* _colorpage;
    KChartParameterConfigPage*_axespage;
    KChartParameter3dConfigPage*_parameter3dpage;
    KChartParameterPieConfigPage*_parameterpiepage;
    KChartFontConfigPage*_parameterfontpage;
    KChartPieConfigPage*_piepage;
    KChartSubTypeChartPage *_subTypePage;
    KChartBackgroundPixmapConfigPage* _backgroundpixpage;
    //KChartComboPage *_hlcChart;
    KChartLegendConfigPage *_parameterLegend;
    KChartHeaderFooterConfigPage *_headerfooterpage;
protected slots:
    virtual void apply();
    virtual void defaults();
};

#endif
