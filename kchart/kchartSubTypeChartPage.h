/*
 * $Id$
 *
 * Copyright 1999-2000 by Matthias Kalle Dalheimer <kalle@kde.org>,
 *   released under Artistic License
 */

#ifndef __KCHARTSUBTYPECHARTPAGE_H__
#define __KCHARTSUBTYPECHARTPAGE_H__

#include <qwidget.h>

class KDChartParams;

class KChartSubTypeChartPage : public QWidget
{
    Q_OBJECT

public:
    KChartSubTypeChartPage( KDChartParams* params, QWidget* parent ) :
        QWidget( parent ), _params( params ) {}
    virtual void init() = 0;
    virtual void apply() = 0;

protected:
    KDChartParams* _params;
};

#endif
