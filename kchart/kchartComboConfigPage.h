/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTCOMBOPAGE_H__
#define __KCHARTCOMBOPAGE_H__

#include <qwidget.h>

#include "kchartparams.h"

class QRadioButton;

class KChartComboPage : public QWidget
{
    Q_OBJECT

public:
    KChartComboPage( KChartParameters* params,QWidget* parent );
    void init();
    void apply();

private:
    KChartParameters* _params;
    QRadioButton *diamond;
    QRadioButton *closeconnected;
    QRadioButton *connecting;
    QRadioButton *icap;
};

#endif
