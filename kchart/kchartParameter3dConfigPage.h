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
class QCheckBox;

class KChartParameter3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameter3dConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();
protected slots:
    void slotChange3DParameter(bool);
private:
    KChartParams* _params;
    KIntNumInput* angle3d;
    KDoubleNumInput* depth;
    QCheckBox* bar3d, *drawShadowColor;
};

#endif
