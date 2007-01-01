/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
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
#include <QWidget>
#include <q3scrollview.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>

/**
  \file KDChartWidget.h

  \brief Provids the entry point into the charting that most
  people will want to use.
  */

class KDChartParams;

class KDCHART_EXPORT KDChartWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( bool activeData READ isActiveData WRITE setActiveData )
    Q_PROPERTY( bool doubleBuffered READ isDoubleBuffered WRITE setDoubleBuffered )

public:
    KDChartWidget( QWidget* parent = 0 );
    KDChartWidget( KDChartParams* params,
                   KDChartTableDataBase* data,
                   QWidget* parent = 0 );
    ~KDChartWidget();

public slots:
    bool isActiveData() const;
    bool isDoubleBuffered() const;

    KDChartParams* params() const;
    KDChartTableDataBase* data() const;

    const KDChartDataRegionList* dataRegions() const { return &_dataRegions; }

    void setActiveData( bool active );
    void setDoubleBuffered( bool doublebuffered );
    void setParams( KDChartParams* params );
    void setData( KDChartTableDataBase* data );
    // use this method to paint to low resolution devices
    void paintTo( QPainter& painter,
                  const QRect* rect = 0 );
    // use this method to paint to high res devices like printers...
    void print( QPainter& painter,
                const QRect* rect = 0  );



signals:
    void dataLeftClicked( uint row, uint col );
    void dataLeftClicked( const QPoint & pnt );
    void dataMiddleClicked( uint row, uint col );
    void dataMiddleClicked( const QPoint & pnt );
    void dataRightClicked( uint row, uint col );
    void dataRightClicked( const QPoint & pnt );
    void dataLeftPressed( uint row, uint col );
    void dataLeftPressed( const QPoint & pnt );
    void dataMiddlePressed( uint row, uint col );
    void dataMiddlePressed( const QPoint & pnt );
    void dataRightPressed( uint row, uint col );
    void dataRightPressed( const QPoint & pnt );
    void dataLeftReleased( uint row, uint col );
    void dataLeftReleased( const QPoint & pnt );
    void dataMiddleReleased( uint row, uint col );
    void dataMiddleReleased( const QPoint & pnt );
    void dataRightReleased( uint row, uint col );
    void dataRightReleased( const QPoint & pnt );
    void barsDisplayed( int barsDisplayed, int barsLeft );

protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void resizeEvent( QResizeEvent* event );

private:
    KDChartParams* _params;
    KDChartTableDataBase* _data;
    bool _activeData;
    bool _doubleBuffered;
    QPixmap _buffer;
    KDChartDataRegion* _mousePressedOnRegion;
    KDChartDataRegionList _dataRegions;
};

#endif
