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
class QCheckBox;

class KChartParams;

class KChartParameterPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterPieConfigPage( KChartParams* params,QWidget* parent );
    void init();
    void apply();
public slots:
    void active3DPie(bool b);
private:
    KChartParams* _params;
    QCheckBox *pie3d;
    QSpinBox *depth,*angle;
};

#endif
