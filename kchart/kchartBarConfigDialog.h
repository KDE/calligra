/*
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KOCHARTBARCONFIGDIALOG_H__
#define __KOCHARTBARCONFIGDIALOG_H__

#include "kchartConfigDialog.h"
#include "kchartDataEditor.h"

class KChartParams;
class KChartBarConfigPage;

class KChartBarConfigDialog : public KChartConfigDialog
{
    Q_OBJECT

public:
    KChartBarConfigDialog( KChartParams* params, 
                           QWidget* parent, int flag, KoChart::Data *dat );

public slots:
    virtual void apply();
    virtual void defaults();

private:
    KChartBarConfigPage* _barpage;
};

#endif
