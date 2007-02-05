/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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

#include <QLabel>
#include <q3frame.h>
#include <qbitmap.h>
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <Q3PtrList>
#include <QPointF>
#include <QRectF>
#include <QPainter>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kiconloader.h>

#include <KoUniColorDialog.h>

#include "vgradientwidget.h"
#include "vcolordlg.h"
#include "vfill.h"
#include "vqpainter.h"
#include "vcursor.h"

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

VGradientWidget::VGradientWidget( const QGradientStops & stops, QWidget* parent, const char* name )
: QWidget( parent ), m_currentStop( -1 )
{
    m_gradient.setStops( stops );

    setObjectName(name);

    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(Qt::NoBrush));
    // TODO: check if this is equivalent with the line underneath. It might need autoFillBackground = true
    //setBackgroundMode( Qt::NoBackground );

    setMinimumSize( 105, 35 );
}

VGradientWidget::~VGradientWidget()
{
}

void VGradientWidget::setStops( const QGradientStops & stops )
{
    m_gradient.setStops( stops );
}

QGradientStops VGradientWidget::stops() const
{
    return m_gradient.stops();
}

void VGradientWidget::paintColorStop( QPainter& p, int x, QColor& color )
{
    QBitmap bitmap;

    // TODO: in Qt3, the bitmap data was loaded with bool isXbitmap = true.
    // Check if this is still correct otherwise in Qt4, QImage::Format monoFormat needs to change.
    bitmap = QBitmap::fromData( QSize(colorStop_width, colorStop_height), colorStop_bits );
    bitmap.setMask( bitmap );
    p.setPen( color );
    p.drawPixmap( x - 4, 1, bitmap );

    // TODO: in Qt3, the bitmap data was loaded with bool isXbitmap = true.
    // Check if this is still correct otherwise in Qt4, QImage::Format monoFormat needs to change.
    bitmap = QBitmap::fromData( QSize(colorStopBorder_width, colorStopBorder_height), colorStopBorder_bits );
    bitmap.setMask( bitmap );
    p.setPen( QColor( "black" ) );
    p.drawPixmap( x - 5, 1, bitmap );
}

void VGradientWidget::paintMidPoint( QPainter& p, int x )
{
    // TODO: in Qt3, the bitmap data was loaded with bool isXbitmap = true.
    // Check if this is still correct otherwise in Qt4, QImage::Format monoFormat needs to change.
    QBitmap bitmap = QBitmap::fromData( QSize(midPoint_width, midPoint_height), midPoint_bits );
    bitmap.setMask( bitmap );
    p.setPen( QColor( "black" ) );
    p.drawPixmap( x - 3, 1, bitmap );
}

void VGradientWidget::paintEvent( QPaintEvent* )
{
    int w = width() - 4;  // available width for gradient and points
    int h = height() - 7; // available height for gradient and points
    int ph = colorStopBorder_height + 2; // point marker height
    int gh = h - ph;       // gradient area height

    QPixmap pixmap( width(), height() );
    QPainter painter( this );

    m_gradient.setStart( QPointF( 2, 2 ) );
    m_gradient.setFinalStop( QPointF( width()-2, 2 ) );

    painter.setBrush( QBrush( SmallIcon( "karbon" ) ) );
    painter.drawRect( QRectF( 2, 2, w, gh ) );

    painter.setBrush( QBrush( m_gradient ) );
    painter.drawRect( QRectF( 2, 2, w, gh ) );

    // TODO: check if this is equivalent with colorGroup().light()
    painter.setBrush( QBrush() );
    painter.setPen( palette().light().color() );

    // light frame around widget
    QRect frame( 1, 1, width()-2, height()-2 );
    painter.drawRect( frame );

    // light line between gradient and point area
    painter.drawLine( QLine( QPoint( 1, 3 + gh ), QPoint( width() - 1, 3 + gh ) ) );

    painter.setPen( colorGroup().dark() );
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
    // clear point area
    // TODO: check if palette().window().color() is equivalent with colorGroup().background()
    painter.fillRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height(), palette().window().color() );

    painter.setClipRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height() );
    painter.translate( m_pntArea.x(), m_pntArea.y() );

    QGradientStops colorStops = m_gradient.stops();
    if( colorStops.count() > 1 )
    {
        foreach( QGradientStop stop, colorStops )
            paintColorStop( painter, (int)( stop.first * m_pntArea.width() ), stop.second );
    }
}

void VGradientWidget::mousePressEvent( QMouseEvent* e )
{
    if( ! m_pntArea.contains( e->x(), e->y() ) )
        return;

    QGradientStops colorStops = m_gradient.stops();

    m_currentStop = -1;

    int x = e->x() - m_pntArea.left();

    for( int i = colorStops.count() - 1; i >= 0; i-- )
    {
        int r = int( colorStops[i].first * m_pntArea.width() );
        if( ( x > r - 5 ) && ( x < r + 5 ) )
        {
            // found ramp point at position
            m_currentStop = i;
            if( e->button() == Qt::LeftButton )
                setCursor( VCursor::horzMove() );
            return;
        }
    }
}

void VGradientWidget::mouseReleaseEvent( QMouseEvent* e )
{
    if( e->button() == Qt::RightButton && m_currentStop >= 0 )
    {
        if( m_pntArea.contains( e->x(), e->y() ) )
        {
            QGradientStops colorStops = m_gradient.stops();
            int x = e->x() - m_pntArea.left();
            // check if we are still above the actual ramp point
            int r = int( colorStops[ m_currentStop ].first * m_pntArea.width() );
            if( ( x > r - 5 ) && ( x < r + 5 ) )
            {
                colorStops.remove( m_currentStop );
                m_gradient.setStops( colorStops );
                update();
                emit changed();
            }
        }
    }
    setCursor( QCursor( Qt::ArrowCursor ) );
}

void VGradientWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
    if( ! m_pntArea.contains( e->x(), e->y() ) )
        return;

    if( e->button() != Qt::LeftButton )
        return;

    QGradientStops colorStops = m_gradient.stops();

    if( m_currentStop >= 0 )
    {
        // ramp point hit -> change color
        KoColor oldColor;
        oldColor.fromQColor( colorStops[m_currentStop].second );

        KoUniColorDialog * d = new KoUniColorDialog( oldColor, this->topLevelWidget() );
        if( d->exec() == QDialog::Accepted )
        {
            colorStops[m_currentStop].second = d->color().toQColor();
            m_gradient.setStops( colorStops );
            update();
            emit changed();
        }
        delete d;
    }
    else if( m_currentStop == -1 )
    {
        KoColor newColor;
        newColor.fromQColor( colorStops[0].second );

        // no point hit -> create new color stop
        KoUniColorDialog * d = new KoUniColorDialog( newColor, this->topLevelWidget() );
        if( d->exec() == QDialog::Accepted )
        {
            m_gradient.setColorAt( (float)( e->x()-m_pntArea.left() ) / m_pntArea.width(), d->color().toQColor() );
            update();
            emit changed();
        }
        delete d;
    }
}

void VGradientWidget::mouseMoveEvent( QMouseEvent* e )
{
    if( e->buttons() & Qt::RightButton )
        return;

    QGradientStops colorStops = m_gradient.stops();

    if( m_currentStop < 0 || m_currentStop >= colorStops.count() )
        return;

    double x = (double)( e->x() - m_pntArea.left() ) / (double)m_pntArea.width();

    // move ramp point
    double minX = m_currentStop > 0 ? colorStops[m_currentStop-1].first : 0.0f;
    double maxX = m_currentStop < colorStops.count()-1 ? colorStops[m_currentStop+1].first : 1.0f;

    // Clip the color stop between to others.
    x = qMin( x, maxX );
    x = qMax( x, minX );
    colorStops[m_currentStop].first = x;
    m_gradient.setStops( colorStops );
    update();
    emit changed();
}

#include "vgradientwidget.moc"
