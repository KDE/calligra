/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTBARSUBTYPECHARTPAGE_H__
#define __KCHARTBARSUBTYPECHARTPAGE_H__

#include <qwidget.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include "kchartparams.h"

class KChartSubTypeChartPage : public QWidget
{
    Q_OBJECT

public:
    KChartSubTypeChartPage( KChartParameters* params,QWidget* parent );
    void init();
    void apply();

private:
    KChartParameters* _params;
    QRadioButton *depth;
    QRadioButton *sum;
    QRadioButton *beside;
    QRadioButton *layer;
    QRadioButton *percent;
};

#endif
