/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTLINE3DCONFIGPAGE_H__
#define __KCHARTLINE3DCONFIGPAGE_H__

#include <qwidget.h>
#include <knuminput.h>

class KChartParams;
class QCheckBox;

class KChartLine3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLine3dConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();
protected slots:
    void slotChange3DParameter(bool);
private:
    KChartParams* _params;
    KIntNumInput* angle3dX;
    KIntNumInput* angle3dY;
    KDoubleNumInput* depth;
    QCheckBox* line3d, *drawShadowColor;
};

#endif
