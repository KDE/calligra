/*
 * Copyright 2001 by Laurent MONTEL <lmontel@mandrakesoft.com>,
 *   released under Artistic License
 */

#ifndef __KCHARTHILOSUBTYPECHARTPAGE_H__
#define __KCHARTHILOSUBTYPECHARTPAGE_H__

#include "kchartSubTypeChartPage.h"

class QLabel;
class QRadioButton;
class KChartParams;

class KDChartParams;

class KChartHiloSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartHiloSubTypeChartPage( KDChartParams* params,
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
