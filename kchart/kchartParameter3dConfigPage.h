/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTPARAMETER3DCONFIGPAGE_H__
#define __KCHARTPARAMETER3DCONFIGPAGE_H__

#include <qwidget.h>
#include <knuminput.h>

class KDChartParams;

class KChartParameter3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameter3dConfigPage( KDChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();

private:
    KDChartParams* _params;
    KIntNumInput* angle3d;
    KDoubleNumInput* depth;
    KIntNumInput* bar_width;
};

#endif
