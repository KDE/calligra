/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTLEGENDCONFIGPAGE_H__
#define __KCHARTLEGENDCONFIGPAGE_H__

#include <qwidget.h>

class KChartParams;
class QRadioButton;
class QLineEdit;
class KColorButton;

class KChartLegendConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLegendConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();

private:
    KChartParams* _params;
    QRadioButton *noLegend,*lTop,*lBottom,*lLeft,*lRight,*lTopLeft,*lTopRight,*lBottomRight,*lBottomLeft;
    QLineEdit *title;
    KColorButton *legendTitleColor, *legendTextColor;
};

#endif
