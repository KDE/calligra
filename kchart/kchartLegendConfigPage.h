/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTLEGENDCONFIGPAGE_H__
#define __KCHARTLEGENDCONFIGPAGE_H__

#include <qwidget.h>

class KDChartParams;
class QRadioButton;

class KChartLegendConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLegendConfigPage( KDChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();

private:
    KDChartParams* _params;
    QRadioButton *noLegend,*lTop,*lBottom,*lLeft,*lRight,*lTopLeft,*lTopRight,*lBottomRight,*lBottomLeft;
};

#endif
