/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETER3DCONFIGPAGE_H__
#define __KCHARTPARAMETER3DCONFIGPAGE_H__

#include <qwidget.h>
#include <qcheckbox.h>
//#include <qlineedit.h>
#include <knuminput.h>

#include "kchartparams.h"

class KChartParameter3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameter3dConfigPage( KChartParameters* params,QWidget* parent );
    void init();
    void apply();

private:
    KChartParameters* _params;
    KIntNumInput* angle3d;
    KDoubleNumInput* depth;
    KIntNumInput* bar_width;
};

#endif
