/*
 * Copyright 2001 by Laurent MONTEL <lmontel@mandrakesoft.com>,
 *   released under Artistic License
 */

#ifndef __KCHARTPOLARSUBTYPECHARTPAGE_H__
#define __KCHARTPOLARSUBTYPECHARTPAGE_H__

#include "kchartSubTypeChartPage.h"

class QLabel;
class QRadioButton;
class KChartParams;

class KDChartParams;

class KChartPolarSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartPolarSubTypeChartPage( KDChartParams* params,
                                QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton* normal;
    QRadioButton* stacked;
    QRadioButton* percent;
    QLabel* exampleLA;
};

#endif
