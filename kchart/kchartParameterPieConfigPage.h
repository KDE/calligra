/*
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERPIECONFIGPAGE_H__
#define __KCHARTPARAMETERPIECONFIGPAGE_H__

#include <qwidget.h>

class QCheckBox;
class QRadioButton;
class QLineEdit;
class QSpinBox;

class KChartParams;

class KChartParameterPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterPieConfigPage( KChartParams* params,QWidget* parent );
    void init();
    void apply();

private:
    KChartParams* _params;
    QLineEdit *title;
};

#endif
