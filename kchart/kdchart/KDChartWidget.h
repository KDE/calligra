/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KDChart library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KDChart licenses may use this file in
** accordance with the KDChart Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#ifndef __KDCHARTWIDGET_H__
#define __KDCHARTWIDGET_H__

#include <KDChartGlobal.h>
#include <KDChartTable.h>
#include <KDChartDataRegion.h>
#include <qwidget.h>
#include <qpixmap.h>


class KDChartParams;

class KDChartWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( bool activeData READ isActiveData WRITE setActiveData )
    Q_PROPERTY( bool doubleBuffered READ isDoubleBuffered WRITE setDoubleBuffered )

public:
    KDChartWidget( KDChartParams* params,
                   KDChartTableData* data,
                   QWidget* parent = 0, const char* name = 0 );
    ~KDChartWidget();

    bool isActiveData() const;
    bool isDoubleBuffered() const;

public slots:
    void setActiveData( bool active );
    void setDoubleBuffered( bool doublebuffered );
    void setParams( KDChartParams* params );
    void setData( KDChartTableData* data );

signals:
    void dataLeftClicked( uint row, uint col );
    void dataMiddleClicked( uint row, uint col );
    void dataRightClicked( uint row, uint col );
    void dataLeftPressed( uint row, uint col );
    void dataMiddlePressed( uint row, uint col );
    void dataRightPressed( uint row, uint col );
    void dataLeftReleased( uint row, uint col );
    void dataMiddleReleased( uint row, uint col );
    void dataRightReleased( uint row, uint col );

protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void resizeEvent( QResizeEvent* event );

private:
    KDChartParams* _params;
    KDChartTableData* _data;
    bool _activeData;
    bool _doubleBuffered;
    QPixmap _buffer;
    KDChartDataRegion* _mousePressedOnRegion;
    KDChartDataRegionList _dataRegions;
};

#endif
