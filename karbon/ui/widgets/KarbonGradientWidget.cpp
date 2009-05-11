/* This file is part of the KDE project
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007,2009 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonGradientWidget.h"
#include "KarbonCursor.h"
#include "KarbonGradientHelper.h"

#include <KLocale>
#include <KIconLoader>
#include <KColorDialog>

#include <QtGui/QLabel>
#include <QtGui/QBitmap>
#include <QtGui/QPaintEvent>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <limits>

#define midPoint_width 7
#define midPoint_height 10
static unsigned char midPoint_bits[] = {
   0x08, 0x08, 0x1c, 0x1c, 0x2a, 0x2a, 0x08, 0x08, 0x08, 0x08
};

#define colorStopBorder_width 11
#define colorStopBorder_height 11
static unsigned char colorStopBorder_bits[] = {
   0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01,
   0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0xff, 0x07
};

#define colorStop_width 9
#define colorStop_height 10
static unsigned char colorStop_bits[] = {
   0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x38, 0x00, 0x38, 0x00, 0x7c, 0x00,
   0x7c, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xff, 0x01
};

KarbonGradientWidget::KarbonGradientWidget( QWidget* parent )
: QWidget( parent ), m_currentStop( -1 ), m_checkerPainter( 4 )
{
    // initialize the gradient with some sane values
    m_stops.append( QGradientStop(0.0, Qt::white));
    m_stops.append( QGradientStop(1.0, Qt::green));

    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(Qt::NoBrush));

    setMinimumSize( 105, 35 );
}

KarbonGradientWidget::~KarbonGradientWidget()
{
}

void KarbonGradientWidget::setStops( const QGradientStops & stops )
{
    m_stops = stops;
    update();
}

QGradientStops KarbonGradientWidget::stops() const
{
    return m_stops;
}

void KarbonGradientWidget::paintColorStop( QPainter& p, int x, const QColor& color )
{
    QBitmap bitmap = QBitmap::fromData( QSize(colorStop_width, colorStop_height), colorStop_bits );
    bitmap.setMask( bitmap );
    p.setPen( color );
    p.drawPixmap( x - 4, 1, bitmap );

    bitmap = QBitmap::fromData( QSize(colorStopBorder_width, colorStopBorder_height), colorStopBorder_bits );
    bitmap.setMask( bitmap );
    p.setPen( Qt::black );
    p.drawPixmap( x - 5, 1, bitmap );
}

void KarbonGradientWidget::paintMidPoint( QPainter& p, int x )
{
    QBitmap bitmap = QBitmap::fromData( QSize(midPoint_width, midPoint_height), midPoint_bits );
    bitmap.setMask( bitmap );
    p.setPen( Qt::black );
    p.drawPixmap( x - 3, 1, bitmap );
}

void KarbonGradientWidget::paintEvent( QPaintEvent* )
{
    int w = width() - 4;  // available width for gradient and points
    int h = height() - 7; // available height for gradient and points
    int ph = colorStopBorder_height + 2; // point marker height
    int gh = h - ph;       // gradient area height

    QPainter painter( this );

    QLinearGradient gradient;
    gradient.setStart( QPointF( 2, 2 ) );
    gradient.setFinalStop( QPointF( width()-2, 2 ) );
    gradient.setStops( m_stops );
    
    m_checkerPainter.paint( painter, QRectF( 2, 2, w, gh ) );

    painter.setBrush( QBrush( gradient ) );
    painter.drawRect( QRectF( 2, 2, w, gh ) );

    painter.setBrush( QBrush() );
    painter.setPen( palette().light().color() );

    // light frame around widget
    QRect frame( 1, 1, width()-2, height()-2 );
    painter.drawRect( frame );

    // light line between gradient and point area
    painter.drawLine( QLine( QPoint( 1, 3 + gh ), QPoint( width() - 1, 3 + gh ) ) );

    painter.setPen( palette().dark().color() );
    // left-top frame around widget
    painter.drawLine( QPoint(), QPoint( 0, height() - 1 ) );
    painter.drawLine( QPoint(), QPoint( width() - 1, 0 ) );

    // right-bottom from around gradient
    painter.drawLine( QPoint( width() - 2, 2 ), QPoint( width() - 2, 2 + gh ) );
    painter.drawLine( QPoint( width() - 2, 2 + gh ), QPoint( 2, 2 + gh ) );

    // upper line around point area
    painter.drawLine( QPoint( 1, height() - 3 - ph ), QPoint( width() - 1, height() - 3 - ph ) );

    // right-bottom line around point area
    painter.drawLine( QPoint( width() - 2, height() - ph - 1 ), QPoint( width() - 2, height() - 2 ) );
    painter.drawLine( QPoint( width() - 2, height() - 2 ), QPoint( 2, height() - 2 ) );

    m_pntArea.setRect( 2, height() - ph - 2, w, ph );
    painter.fillRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height(), palette().window().color() );

    painter.setClipRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height() );
    painter.translate( m_pntArea.x(), m_pntArea.y() );

    foreach( const QGradientStop & stop, m_stops )
        paintColorStop( painter, (int)( stop.first * m_pntArea.width() ), stop.second );
}

void KarbonGradientWidget::mousePressEvent( QMouseEvent* e )
{
    m_currentStop = colorStopFromPosition( QPoint(e->x(), e->y()) );
    setCursor( m_currentStop == -1 ? QCursor( Qt::ArrowCursor ) : KarbonCursor::horzMove() );
}

void KarbonGradientWidget::mouseReleaseEvent( QMouseEvent* e )
{
    if( e->button() == Qt::RightButton && m_currentStop >= 0 )
    {
        if( m_pntArea.contains( e->x(), e->y() ) )
        {
            int x = e->x() - m_pntArea.left();
            // check if we are still above the actual ramp point
            int r = int( m_stops[ m_currentStop ].first * m_pntArea.width() );
            if( ( x > r - 5 ) && ( x < r + 5 ) )
            {
                m_stops.remove( m_currentStop );
                update();
                emit changed();
            }
        }
    }
    setCursor( QCursor( Qt::ArrowCursor ) );
}

void KarbonGradientWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
    if( ! m_pntArea.contains( e->x(), e->y() ) )
        return;

    if( e->button() != Qt::LeftButton )
        return;

    if( m_currentStop >= 0 )
    {
        // color stop hit -> change color
        int result = KColorDialog::getColor( m_stops[m_currentStop].second, this );
        if ( result == KColorDialog::Accepted ) {
            update();
            emit changed();
        }
    }
    else if( m_currentStop == -1 )
    {
        // no color stop hit -> new color stop
        qreal newStopPosition = static_cast<qreal>( e->x()-m_pntArea.left() ) / m_pntArea.width();
        QColor newStopColor = KarbonGradientHelper::colorAt( newStopPosition, m_stops );
        int result = KColorDialog::getColor( newStopColor, this );
        if ( result == KColorDialog::Accepted ) {
            m_stops.append( QGradientStop(newStopPosition, newStopColor) );
            update();
            emit changed();
        }
    }
}

void KarbonGradientWidget::mouseMoveEvent( QMouseEvent* e )
{
    if( e->buttons() & Qt::RightButton )
        return;

    // do we have a current color stop?
    if ( m_currentStop >= 0 && m_currentStop < m_stops.count() ) {
        int newPixelPos = e->x() - m_pntArea.left();
        int oldPixelPos = static_cast<int>( m_stops[m_currentStop].first * m_pntArea.width() );
        // check if we really have changed the pixel based position
        if( newPixelPos == oldPixelPos )
            return;
        
        qreal newPosition = static_cast<qreal>( newPixelPos ) / static_cast<qreal>(m_pntArea.width());
        m_currentStop = moveColorStop( m_currentStop, newPosition );
        
        update();
        emit changed();
    }
    else {
        int colorStop = colorStopFromPosition( QPoint(e->x(), e->y()) );
        setCursor( colorStop == -1 ? QCursor( Qt::ArrowCursor ) : KarbonCursor::horzMove() );
    }
}

int KarbonGradientWidget::moveColorStop( int stopIndex, qreal newPosition )
{
    // get old position of the color stop we want to move
    const qreal oldPosition = m_stops[stopIndex].first;
    
    // clip new position to [0,1] interval
    newPosition = qBound(qreal(0.0), newPosition, qreal(1.0));
    
    // initialize new stop index to the current index
    int newStopIndex = stopIndex;
    
    // we move to the left, so we swap color stops as long as our new position 
    // is smaller or equal to the previous color stop position
    if( newPosition < oldPosition && m_currentStop > 0 ) {
        for( int i = m_currentStop-1; i >= 0; i-- ) {
            const qreal prevStopPos = m_stops[i].first;
            if( newPosition <= prevStopPos && prevStopPos > 0.0 ) {
                qSwap( m_stops[i], m_stops[newStopIndex] );
                newStopIndex = i;
            }
            else
                break;
        }
    }
    // we move to the right, so we swap color stops as long as our new position 
    // is greater or equal to the next color stop position
    else if( newPosition > oldPosition && m_currentStop < m_stops.count()-1 ) { 
        for( int i = m_currentStop+1; i < m_stops.count(); ++i ) {
            const qreal nextStopPos = m_stops[i].first;
            if( newPosition >= nextStopPos && nextStopPos < 1.0 ) {
                qSwap( m_stops[i], m_stops[newStopIndex] );
                newStopIndex = i;
            }
            else
                break;
        }
    }

    // work around qt bug: make sure we do not have color stops with same position
    if( newStopIndex < stopIndex && newPosition == m_stops[newStopIndex+1].first ) {
        // if we have decreased the stop index and the new position is equal to the
        // position of the next stop we move the new position a tiny bit to the left
        newPosition -= std::numeric_limits<qreal>::epsilon();
    }
    else if( newStopIndex > stopIndex && newPosition == m_stops[newStopIndex-1].first ) {
        // if we have increased the stop index and the new position is equal to the
        // position of the previous stop we move the new position a tiny bit to the right
        newPosition += std::numeric_limits<qreal>::epsilon();
    }
    else {
        // we have not changed the stop index, but we have to check if we our new 
        // stop position is not equal to the stop at 0.0 or 1.0 
        if( newStopIndex == 1 && newPosition == 0.0 )
            newPosition += std::numeric_limits<qreal>::epsilon();
        if( newStopIndex == m_stops.count()-2 && newPosition == 1.0 )
            newPosition -= std::numeric_limits<qreal>::epsilon();
    }

    // finally set the new stop position
    m_stops[newStopIndex].first = newPosition;

    return newStopIndex;
}

int KarbonGradientWidget::colorStopFromPosition( const QPoint &mousePos )
{
    if( ! m_pntArea.contains( mousePos ) )
        return -1;
    
    int x = mousePos.x() - m_pntArea.left();
    
    for( int i = m_stops.count() - 1; i >= 0; i-- )
    {
        int r = int( m_stops[i].first * m_pntArea.width() );
        if( ( x > r - 5 ) && ( x < r + 5 ) )
        {
            // found color stop at position
            return i;
        }
    }
    
    return -1;
}

#include "KarbonGradientWidget.moc"
