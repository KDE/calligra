/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERCONFIGPAGE_H__
#define __KCHARTPARAMETERCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbtn.h>

class QSpinBox;
class QCheckBox;
class QLineEdit;

class KDChartParams;

class KChartParameterConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterConfigPage( KDChartParams* params, QWidget* parent );
    void init();
    void apply();
public slots:
    void changeState( bool );
    void changeXaxisState( bool );
    void changeFont();
private:
    KDChartParams* _params;
    QCheckBox *grid;
    QCheckBox *border;
    QCheckBox *xaxis;
    QCheckBox *yaxis;
    QCheckBox *yaxis2;
    QCheckBox *shelf;
    QCheckBox *xlabel;
    QCheckBox *cross;
    QCheckBox *llabel;
    QLineEdit *title;
    QLineEdit *xtitle;
    QLineEdit *ytitle;
    QLineEdit *ylabel_fmt;
    QLineEdit *ytitle2;
    QLineEdit *ylabel2_fmt;
    QLineEdit *annotation;
    //KIntNumInput *element;
    QSpinBox *element;
    KColorButton *color;
    QPushButton *font;
    QFont annotationFont;
};

#endif
