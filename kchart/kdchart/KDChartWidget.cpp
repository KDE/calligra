/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#include <KDChartWidget.h>
#include <KDChart.h>
#include <KDChartParams.h>
#include <KDChartTable.h>
#ifndef KDCHART_MASTER_CVS
#include "KDChartWidget.moc"
#endif

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
    setDoubleBuffered( true );
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
#ifdef USE_EXCEPTIONS
    try {
#endif
        if( _doubleBuffered ) {
            // if double-buffering, paint onto the pixmap and copy
            // afterwards
            _buffer.fill( backgroundColor() );
            QPainter painter( &_buffer );
            KDChart::paint( &painter, _params, _data, &_dataRegions );
            bitBlt( this, event->rect().topLeft(), &_buffer, event->rect() );
        } else {
            // if not double-buffering, paint directly into the window
            QPainter painter( this );
            KDChart::paint( &painter, _params, _data, &_dataRegions );
        }
#ifdef USE_EXCEPTIONS
    } catch ( ... ) {
        ::qDebug( "Exception occurred during chart painting" );
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
    QPtrListIterator < KDChartDataRegion > it( _dataRegions );
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
    QPtrListIterator < KDChartDataRegion > it( _dataRegions );
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
   \internal
*/
void KDChartWidget::resizeEvent( QResizeEvent* event )
{
    // if we use double-buffering, resize the buffer to the new size,
    // otherwise leave it alone
    if( _doubleBuffered )
        _buffer.resize( size() );
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


/**
   If \a doublebuffered is true, the widget will double-buffer
   everything while drawing which reduces flicker a lot, but requires
   more memory as an off-screen buffer of the same size as the widget
   needs to be kept around. However, in most cases, it is worth
   spending the extra memory. Double-buffering is on by
   default. Turning double-buffering on or off does not trigger a
   repaint.
   
   \param doublebuffered if true, turns double-buffering on, if false,
   turns double-buffering off
   \sa isDoubleBuffered
*/
void KDChartWidget::setDoubleBuffered( bool doublebuffered ) 
{
    _doubleBuffered = doublebuffered;
    if( doublebuffered ) {
        // turn double-buffering on
        // resize the buffer to the size of the widget
        _buffer.resize( size() );
    } else {
        // turn double-buffering off
        // minimize the buffer so that it does not take any memory
        _buffer.resize( 0, 0 );
    }
}


/**
   Returns whether the widget uses double-buffering for drawing. See
   \a setDoubleBuffered() for an explanation of double-buffering.
   
   \return true if double-buffering is turned on, false otherwise
*/
bool KDChartWidget::isDoubleBuffered() const
{
    return _doubleBuffered;
}


/**
    Set an entire new parameter set.
    (Normally you might prefer modifying the existing parameters
     rather than specifying a new set.)
*/
void KDChartWidget::setParams( KDChartParams* params )
{
    _params = params;
}

/**
    Set an entire new data table.
*/
void KDChartWidget::setData( KDChartTableData* data )
{
    _data = data;
}
