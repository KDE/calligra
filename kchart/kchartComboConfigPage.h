/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTCOMBOPAGE_H__
#define __KCHARTCOMBOPAGE_H__

#include <qwidget.h>

class QRadioButton;

class KDChartParams;

class KChartComboPage : public QWidget
{
    Q_OBJECT

public:
    KChartComboPage( KDChartParams* params,QWidget* parent );
    void init();
    void apply();

private:
    KDChartParams* _params;
    QRadioButton *diamond;
    QRadioButton *closeconnected;
    QRadioButton *connecting;
    QRadioButton *icap;
};

#endif
