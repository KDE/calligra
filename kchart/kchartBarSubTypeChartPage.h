/*
 * $Id$
 *
 * Copyright 1999-2000 by Matthias Kalle Dalheimer <kalle@kde.org>,
 *   released under Artistic License
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#ifndef __KCHARTBARSUBTYPECHARTPAGE_H__
#define __KCHARTBARSUBTYPECHARTPAGE_H__

#include "kchartSubTypeChartPage.h"

class QLabel;
class QRadioButton;

class KDChartParams;

class KChartBarSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartBarSubTypeChartPage( KDChartParams* params, QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton *depth;
    QRadioButton *beside;
    QRadioButton *layer;
    QRadioButton *percent;
	QLabel* exampleLA;
};

#endif
