/*
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERPOLARCONFIGPAGE_H__
#define __KCHARTPARAMETERPOLARCONFIGPAGE_H__

#include <qwidget.h>

class QCheckBox;
class QRadioButton;
class QLineEdit;
class QSpinBox;
class QCheckBox;

class KChartParams;

class KChartParameterPolarConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterPolarConfigPage( KChartParams* params,QWidget* parent );
    void init();
    void apply();
private:
    KChartParams* _params;
    QCheckBox *polarMarker;
    QCheckBox *showCircularLabel;
    QSpinBox *angle, *lineWidth;
};

#endif
