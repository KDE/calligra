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

#include <KDChartWidget.h>
#include <KDChart.h>
#include <KDChartParams.h>
#include <KDChartTable.h>

#include <qpainter.h>

/**
   \class KDChartWidget KDChartWidget.h

   \brief The entry point into the charting that most people will want
   to use.

   Simply create a KChartWidget in your application and put it where
   you want in your widget hierarchy and create a KChartParams object
   that specifies how the chart should be drawn.
*/


/**
   Constructor. Stores the chart parameters.

   \param params the specification of the chart
   \param data the data to be displayed as a chart
   \param parent the widget parent; passed on to QWidget
   \param the widget name; passed on to QWidget
*/

KDChartWidget::KDChartWidget( KDChartParams* params,
                              KDChartTableData* data,
                              QWidget* parent, const char* name ) :
    QWidget( parent, name ),
_params( params ),
_data( data ),
_activeData( false ),
_mousePressedOnRegion( 0 )
{
    _dataRegions.setAutoDelete( true );
}


/**
   Destructor.
*/
KDChartWidget::~KDChartWidget()
{
    // delete any regions that might still be registered
    _dataRegions.clear();
}

void KDChartWidget::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
#ifdef USE_EXCEPTIONS
    try {
#endif
        KDChart::paint( &painter, _params, _data,
                        _activeData ? &_dataRegions : 0 );
#ifdef USE_EXCEPTIONS
    } catch ( ... ) {
        ::qDebug( "Exception occurred during chart processing" );
        throw;
    }
#endif
}


/**
  \internal
*/
void KDChartWidget::mousePressEvent( QMouseEvent* event )
{
    if ( !_activeData )
        return ;

    _mousePressedOnRegion = 0;
    KDChartDataRegion* current = 0;
    QListIterator < KDChartDataRegion > it( _dataRegions );
    while ( ( current = it.current() ) ) {
        ++it;
        if ( current->region.contains( event->pos() ) ) {
            _mousePressedOnRegion = current;
            if ( event->button() == LeftButton )
                emit dataLeftPressed( current->row,
                                      current->col );
            else if ( event->button() == MidButton )
                emit dataMiddlePressed( current->row,
                                        current->col );
            else
                emit dataRightPressed( current->row,
                                       current->col );
            break;
        }
    }
}

/**
   \internal
*/
void KDChartWidget::mouseReleaseEvent( QMouseEvent* event )
{
    if ( !_activeData )
        return ;

    KDChartDataRegion* current = 0;
    QListIterator < KDChartDataRegion > it( _dataRegions );
    while ( ( current = it.current() ) ) {
        ++it;
        if ( current->region.contains( event->pos() ) ) {
            if ( event->button() == LeftButton ) {
                emit dataLeftReleased( current->row,
                                       current->col );
                if ( _mousePressedOnRegion == current )
                    emit dataLeftClicked( current->row,
                                          current->col );
            } else if ( event->button() == MidButton ) {
                emit dataMiddleReleased( current->row,
                                         current->col );
                if ( _mousePressedOnRegion == current )
                    emit dataMiddleClicked( current->row,
                                            current->col );
            } else {
                emit dataRightReleased( current->row,
                                        current->col );
                if ( _mousePressedOnRegion == current )
                    emit dataRightClicked( current->row,
                                           current->col );
            }
        }
    }
}

/**
   If \a active is true, this widget reports mouse presses, releases
   and clicks on the data segments it displays. This can slow down the
   display process, so this is turned off by default.

   If active data reporting is turned on when the widget is already
   shown, data will be reported after the next repaint(). Call
   repaint() explicitly if necessary.

   Active data is currently supported for bar, pie, and line charts
   (the latter only with markers, as trying to hit the line would be
   too difficult for the user anyway).

   \param active if true, the widget reports mouse events
   \sa isActiveData()
*/
void KDChartWidget::setActiveData( bool active )
{
    _activeData = active;
}


/**
   Returns true if the widget is configured to report mouse
   events. The default is not to report mouse events.

   \return true if the widget is configured to report mouse events,
   false otherwise
   \sa setActiveData()
*/
bool KDChartWidget::isActiveData() const
{
    return _activeData;
}
