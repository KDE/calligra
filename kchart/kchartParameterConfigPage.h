/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERCONFIGPAGE_H__
#define __KCHARTPARAMETERCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbutton.h>

class QSpinBox;
class QCheckBox;
class QLineEdit;

class KChartParams;

class KChartParameterConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterConfigPage( KChartParams* params, QWidget* parent );
    void init();
    void apply();
public slots:
    void changeXaxisState( bool );
private:
    KChartParams* _params;
    QCheckBox *grid;
    QCheckBox *xaxis;
    QCheckBox *yaxis;
    QCheckBox *yaxis2;
    QCheckBox *xlabel;
    QCheckBox *lineMarker;
    QCheckBox *llabel;
    QLineEdit *xtitle;
    QLineEdit *ytitle;
    QLineEdit *ylabel_fmt;
    QLineEdit *ytitle2;
    QLineEdit *ylabel2_fmt;
    QLineEdit *annotation;
};

#endif
