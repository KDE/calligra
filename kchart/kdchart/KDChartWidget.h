/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

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
    Q_PROPERTY( bool activeData READ isActiveData WRITE setActiveData );
    Q_PROPERTY( bool doubleBuffered READ isDoubleBuffered WRITE setDoubleBuffered );

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
