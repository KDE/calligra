/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTWIDGET_H__
#define __KDCHARTWIDGET_H__

#include <KDChartTable.h>
#include <KDChartDataRegion.h>
#include <qwidget.h>

class KDChartParams;

class KDChartWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( bool activeData READ isActiveData WRITE setActiveData )

public:
    KDChartWidget( KDChartParams* params,
                   KDChartTableData* data,
                   QWidget* parent = 0, const char* name = 0 );
    ~KDChartWidget();

    bool isActiveData() const;

public slots:
    void setActiveData( bool active );

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

private:
    KDChartParams* _params;
    KDChartTableData* _data;
    bool _activeData;
    KDChartDataRegion* _mousePressedOnRegion;
    KDChartDataRegionList _dataRegions;
};

#endif
