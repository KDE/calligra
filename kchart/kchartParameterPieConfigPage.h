/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETERPIECONFIGPAGE_H__
#define __KCHARTPARAMETERPIECONFIGPAGE_H__

#include <qwidget.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include "kchartparams.h"

class KChartParameterPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterPieConfigPage( KChartParameters* params,QWidget* parent );
    void init();
    void apply();

public slots:
    void changeState(bool );	
private:
    KChartParameters* _params;
    QRadioButton *_typeNone;
    QRadioButton *_typeAbove;
    QRadioButton *_typeBelow;
    QRadioButton *_typeRight;
    QRadioButton *_typeLeft;
    QCheckBox *lineLabel;
    QLineEdit *title;
    QSpinBox *dist;  
};

#endif
