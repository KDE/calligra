/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERCONFIGPAGE_H__
#define __KCHARTPARAMETERCONFIGPAGE_H__

#include <qwidget.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#include "kchartparams.h"

class KChartParameterConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterConfigPage( KChartParameters* params,QWidget* parent );
    void init();
    void apply();
public slots:
    void changeState( bool );   
private:
    KChartParameters* _params;
    QCheckBox *grid;
    QCheckBox *border;
    QCheckBox *xaxis;
    QCheckBox *yaxis;
    QCheckBox *yaxis2;
    QCheckBox *shelf;
    QCheckBox *xlabel;
    QLineEdit *title;
    QLineEdit *xtitle;
    QLineEdit *ytitle;
    QLineEdit *ylabel_fmt;
    QLineEdit *ytitle2;  
    QLineEdit *ylabel2_fmt;
};

#endif
