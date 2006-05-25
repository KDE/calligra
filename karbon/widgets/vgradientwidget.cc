/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
//Added by qt3to4:
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <Q3PtrList>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <qpainter.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vcolordlg.h"
#include "vfill.h"
#include "vkopainter.h"
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

VGradientWidget::VGradientWidget( VGradient& gradient, QWidget* parent, const char* name )
		: QWidget( parent, name ), m_gradient( &gradient )
{
	setBackgroundMode( Qt::NoBackground );
	setMinimumSize( 105, 35 );
} // VGradientWidget::VGradientWidget

VGradientWidget::~VGradientWidget()
{
} // VGradientWidget::~VGradientWidget

void VGradientWidget::paintColorStop( QPainter& p, int x, VColor& color )
{
	QBitmap bitmap;

	bitmap = QBitmap( colorStop_width, colorStop_height, colorStop_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( color );
	p.drawPixmap( x - 4, 1, bitmap );

	bitmap = QBitmap( colorStopBorder_width, colorStopBorder_height, colorStopBorder_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( Qt::black );
	p.drawPixmap( x - 5, 1, bitmap );
} // VGradientWidget::paintColorStop

void VGradientWidget::paintMidPoint( QPainter& p, int x )
{
	QBitmap bitmap( midPoint_width, midPoint_height, midPoint_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( Qt::black );
	p.drawPixmap( x - 3, 1, bitmap );
} // VGradientWidget::paintMidPoint

void VGradientWidget::paintEvent( QPaintEvent* )
{
	int w = width() - 4;  // available width for gradient and points
	int h = height() - 7; // available height for gradient and points
	int ph = colorStopBorder_height + 2; // point marker height
	int gh = h - ph;       // gradient area height

	QPixmap pixmap( width(), height() );
	VKoPainter gp( &pixmap, width(), height() );
	gp.setRasterOp( Qt::XorROP );
	VGradient gradient( *m_gradient );
	gradient.setType( VGradient::linear );
	gradient.setOrigin( KoPoint( 2, 2 ) );
	gradient.setFocalPoint( KoPoint( 2, 2 ) );
	gradient.setVector( KoPoint( 2 + w, 2 ) );
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", K3Icon::Small ) );
	fill.setType( VFill::patt );
	gp.setBrush( fill );
	gp.drawRect( KoRect( 2, 2, w, gh ) );
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	gp.setBrush( fill );
	gp.drawRect( KoRect( 2, 2, w, gh ) );
	gp.end();

	QPainter p( &pixmap );

	p.setPen( colorGroup().light() );
	// light frame around widget
	p.moveTo( 1, height() - 1 );
	p.lineTo( 1, 1 );
	p.lineTo( width() - 1, 1 );
	p.lineTo( width() - 1, height() - 1 );
	p.lineTo( 1, height() - 1 );

	// light line between gradient and point area
	p.moveTo( 1, 3 + gh );
	p.lineTo( width() - 1, 3 + gh );

	p.setPen( colorGroup().dark() );
	// left-top frame around widget
	p.moveTo( 0, height() - 1 );
	p.lineTo( 0, 0 );
	p.lineTo( width() - 1, 0 );

	// right-bottom from around gradient
	p.moveTo( width() - 2, 2 );
	p.lineTo( width() - 2, 2 + gh );
	p.lineTo( 2, 2 + gh );

	// upper line around point area
	p.moveTo( 1, height() - 3 - ph );
	p.lineTo( width() - 1, height() - 3 - ph );

	// right-bottom line around point area
	p.moveTo( width() - 2, height() - ph - 1 );
	p.lineTo( width() - 2, height() - 2 );
	p.lineTo( 2, height() - 2 );
	
	m_pntArea.setRect( 2, height() - ph - 2, w, ph );
	// clear point area
	p.fillRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height(), colorGroup().background() );

	p.setClipRect( m_pntArea.x(), m_pntArea.y(), m_pntArea.width(), m_pntArea.height() );
	p.translate( m_pntArea.x(), m_pntArea.y() );
	
	Q3PtrList<VColorStop>& colorStops = m_gradient->m_colorStops;
	if( colorStops.count() > 1 )
	{
		VColorStop* stop, *nextstop;
		for( stop = colorStops.first(), nextstop = colorStops.next();
				nextstop; stop = nextstop, nextstop = colorStops.next() )
		{
			paintColorStop( p, (int)( stop->rampPoint * m_pntArea.width() ), stop->color );
			paintMidPoint( p, (int)(( stop->rampPoint + ( nextstop->rampPoint - stop->rampPoint ) * stop->midPoint ) * m_pntArea.width() ) );
		}
		paintColorStop( p, int( stop->rampPoint * w ), stop->color );
	}
	p.end();
	bitBlt( this, 0, 0, &pixmap, 0, 0, width(), height() );
} // VGradientWidget::paintEvent

void VGradientWidget::mousePressEvent( QMouseEvent* e )
{
	if( ! m_pntArea.contains( e->x(), e->y() ) )
		return;

	Q3PtrList<VColorStop>& colorStops = m_gradient->m_colorStops;

	currentPoint = 0;

	int x = e->x() - m_pntArea.left();

	int i = colorStops.count() - 1;
	VColorStop *stop, *nextstop = 0;
	for( stop = colorStops.last(); i >= 0; i--, stop = colorStops.prev() )
	{
		int r = int( stop->rampPoint * m_pntArea.width() );
		if( nextstop )
		{
			int m = int( ( stop->rampPoint + ( nextstop->rampPoint - stop->rampPoint ) * stop->midPoint ) *  m_pntArea.width() );
			if( ( x > m - 5 ) && ( x < m + 5 ) )
			{
				// found mid point at position
				currentPoint = 2 * i + 2;
				if( e->button() == Qt::LeftButton )
					setCursor( VCursor::horzMove() );
				return;
			}
		}
		if( ( x > r - 5 ) && ( x < r + 5 ) )
		{
			// found ramp point at position
			currentPoint = 2 * i + 1;
			if( e->button() == Qt::LeftButton )
				setCursor( VCursor::horzMove() );
			return;
		}

		nextstop = stop;
	}
} // VGradientWidget::mousePressEvent

void VGradientWidget::mouseReleaseEvent( QMouseEvent* e )
{
	if( e->button() == Qt::RightButton && currentPoint )
	{
		if( m_pntArea.contains( e->x(), e->y() ) && ( currentPoint % 2 == 1 ) )
		{
			int x = e->x() - m_pntArea.left();
			// check if we are still above the actual ramp point
			int r = int( m_gradient->m_colorStops.at( int(0.5 * currentPoint) )->rampPoint * m_pntArea.width() );
			if( ( x > r - 5 ) && ( x < r + 5 ) )
			{
				m_gradient->m_colorStops.remove( int(0.5 * currentPoint) );
				update();
				emit changed();
			}
		}
	}
	setCursor( QCursor( Qt::ArrowCursor ) );
} // VGradientWidget::mouseReleaseEvent

void VGradientWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
	if( ! m_pntArea.contains( e->x(), e->y() ) )
		return;

	if( e->button() != Qt::LeftButton )
		return;

	if( currentPoint % 2 == 1 )
	{
		// ramp point hit -> change color
		VColorDlg* d = new VColorDlg( m_gradient->m_colorStops.at( currentPoint / 2 )->color, this->topLevelWidget() );
		if( d->exec() == QDialog::Accepted )
		{
			m_gradient->m_colorStops.at( currentPoint / 2 )->color = d->Color();
			update();
			emit changed();
		}
		delete d;
	}
	else if( currentPoint == 0 )
	{
		// now point hit -> create new color stop
		VColorDlg* d = new VColorDlg( m_gradient->m_colorStops.at( 0 )->color, this->topLevelWidget() );
		if( d->exec() == QDialog::Accepted )
		{
			m_gradient->addStop( d->Color(), (float)( e->x() - 2 ) / ( m_pntArea.width() ), 0.5 );
			update();
			emit changed();
		}
		delete d;
	}
} // VGradientWidget::mouseDoubleClickEvent

void VGradientWidget::mouseMoveEvent( QMouseEvent* e )
{
	if( e->state() & Qt::RightButton )
		return;

	Q3PtrList<VColorStop>& colorStops = m_gradient->m_colorStops;

	if( currentPoint >= colorStops.count() * 2 )
		return;

	int x = e->x() - m_pntArea.left();

	if( currentPoint % 2 == 1 )
	{
		// move ramp point
		int actRP = int( 0.5 * ( currentPoint - 1 ) );
		int prevRP = actRP - 1;
		int nextRP = int( 0.5 * ( currentPoint + 1 ) );
		// Clip the color stop between to others.
		x = qMin( x, ( actRP < int( colorStops.count() - 1 ) ) ? int( colorStops.at( nextRP )->rampPoint * m_pntArea.width() ) : m_pntArea.width() );
		x = qMax( x, ( actRP > 0 ) ? int( colorStops.at( prevRP )->rampPoint * m_pntArea.width() ) : 0 );
		colorStops.at( actRP )->rampPoint = (float)( x ) / m_pntArea.width();
		update();
		emit changed();
	}
	else if( currentPoint > 0 )
	{
		// move mid point
		int prevRP = int( 0.5 * ( currentPoint - 1 ) );
		int nextRP = int( 0.5 * ( currentPoint + 1 ) );
		// Clip the mid point between to ramp points.
		x = qMin( x, int( colorStops.at( nextRP )->rampPoint * m_pntArea.width() ) );
		x = qMax( x, int( colorStops.at( prevRP )->rampPoint * m_pntArea.width() ) );
		colorStops.at( prevRP )->midPoint = ( (float)( x ) / m_pntArea.width() - ( colorStops.at( prevRP )->rampPoint ) ) / ( colorStops.at( nextRP )->rampPoint - colorStops.at( prevRP )->rampPoint );
		update();
		emit changed();
	}
} // VGradientWidget::mouseMoveEvent

#include "vgradientwidget.moc"
