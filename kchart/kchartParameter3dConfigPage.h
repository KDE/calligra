/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETER3DCONFIGPAGE_H__
#define __KCHARTPARAMETER3DCONFIGPAGE_H__

#include <qwidget.h>
#include <knuminput.h>

class KChartParams;

class KChartParameter3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameter3dConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();

private:
    KChartParams* _params;
    KIntNumInput* angle3d;
    KDoubleNumInput* depth;
};

#endif
