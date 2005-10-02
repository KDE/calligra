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

#include <qlabel.h>
#include <qframe.h>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vcolordlg.h"
#include "vfill.h"
#include "vkopainter.h"

static unsigned char midPoint_bits[] =
{
	0x4, 0x4, 0xe, 0xe, 0x15, 0x15, 0x4, 0x4
};

static unsigned char colorStopBorder_bits[] =
{
	0x8, 0x14, 0x14, 0x22, 0x22, 0x41, 0x41, 0x7f
};

static unsigned char colorStop_bits[] =
{
	0x0, 0x4, 0x4, 0xe, 0xe, 0x1f, 0x1f
};

VGradientWidget::VGradientWidget( VGradient*& gradient, QWidget* parent, const char* name )
		: QWidget( parent, name ), m_lpgradient( &gradient )
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

	bitmap = QBitmap( 5, 8, colorStop_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( color );
	p.drawPixmap( x - 2, 1, bitmap );

	bitmap = QBitmap( 7, 8, colorStopBorder_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( Qt::black );
	p.drawPixmap( x - 3, 1, bitmap );
} // VGradientWidget::paintColorStop

void VGradientWidget::paintMidPoint( QPainter& p, int x )
{
	QBitmap bitmap( 5, 8, midPoint_bits, true );
	bitmap.setMask( bitmap );
	p.setPen( Qt::black );
	p.drawPixmap( x - 2, 1, bitmap );
} // VGradientWidget::paintMidPoint

void VGradientWidget::paintEvent( QPaintEvent* )
{
	QPixmap pixmap( width(), height() );
	VKoPainter gp( &pixmap, width(), height() );
	gp.setRasterOp( Qt::XorROP );
	VGradient gradient( **m_lpgradient );
	gradient.setType( VGradient::linear );
	gradient.setOrigin( KoPoint( 2, 2 ) );
	gradient.setFocalPoint( KoPoint( 2, 2 ) );
	gradient.setVector( KoPoint( width() - 3, 2 ) );
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath( "karbon.png", KIcon::Small ) );
	fill.setType( VFill::patt );
	gp.setBrush( fill );
	gp.drawRect( KoRect( 2, 2, width(), height() - 14 ) );
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	gp.setBrush( fill );
	gp.drawRect( KoRect( 2, 2, width(), height() - 14 ) );
	gp.end();

	QPainter p( &pixmap );

	p.setPen( colorGroup().light() );
	p.moveTo( 1, height() - 17 );
	p.lineTo( 1, 1 );
	p.lineTo( width() - 1, 1 );
	p.lineTo( width() - 1, height() - 1 );
	p.lineTo( 1, height() - 1 );
	p.moveTo( 2, height() - 2 );
	p.lineTo( width() - 2, height() - 1 );
	p.lineTo( width() - 2, height() - 14 );
	p.moveTo( 1, height() - 15 );
	p.lineTo( width() - 2, height() - 15 );
	p.moveTo( width() - 2, height() - 2 );
	p.moveTo( 2, height() - 2 );
	p.setPen( colorGroup().dark() );
	p.moveTo( 0, height() - 1 );
	p.lineTo( 0, 0 );
	p.lineTo( width() - 1, 0 );
	p.moveTo( width() - 2, 2 );
	p.lineTo( width() - 2, height() - 16 );
	p.lineTo( 2, height() - 16 );
	p.moveTo( 1, height() - 14 );
	p.lineTo( width() - 1, height() - 14 );
	p.moveTo( width() - 2, height() - 13 );
	p.lineTo( 1, height() - 13 );
	p.lineTo( 1, height() - 2 );
	p.fillRect( 2, height() - 12, width() - 4, 10, colorGroup().background() );

	p.setClipRect( 2, height() - 12, width() - 4, 10 );
	p.translate( 2, height() - 12 );
	int w = width() - 5;

	QPtrList<VColorStop>& colorStops = ( *m_lpgradient )->m_colorStops;
	if( colorStops.count() > 1 )
	{
		VColorStop* stop, *nextstop;
		for( stop = colorStops.first(), nextstop = colorStops.next();
				nextstop; stop = nextstop, nextstop = colorStops.next() )
		{
			paintColorStop( p, (int)( stop->rampPoint * w ), stop->color );
			paintMidPoint( p, (int)(( stop->rampPoint + ( nextstop->rampPoint - stop->rampPoint ) * stop->midPoint ) * w ) );
		}
		paintColorStop( p, int( stop->rampPoint * w ), stop->color );
	}
	p.end();
	bitBlt( this, 0, 0, &pixmap, 0, 0, width(), height() );
} // VGradientWidget::paintEvent

void VGradientWidget::mousePressEvent( QMouseEvent* e )
{
	if( !( ( e->y() > height() - 14 ) && ( e->y() < height() - 2 ) && ( e->x() > 2 ) && ( e->x() < width() - 3 ) ) )
		return;

	QPtrList<VColorStop>& colorStops = ( *m_lpgradient )->m_colorStops;

	currentPoint = 0;

	int i = colorStops.count() - 1;
	int r, m;
	VColorStop *stop, *nextstop = 0;
	for( stop = colorStops.last(); i >= 0; i--, stop = colorStops.prev() )
	{
		r = int( stop->rampPoint * ( width() - 4 ) );
		if( nextstop )
		{
			m = int( stop->rampPoint + ( nextstop->rampPoint - stop->rampPoint ) * stop->midPoint * ( width() - 4 ) );
			if( ( e->x() - 2 > m - 4 ) && ( e->x() - 2 < m + 4 ) )
			{
				currentPoint = 2*i + 2;
				return;
			}
		}
		if( ( e->x() - 2 > r - 5 ) && ( e->x() - 2 < r + 5 ) )
		{
			currentPoint = 2*i + 1;
			return;
		}

		nextstop = stop;
	}
} // VGradientWidget::mousePressEvent

void VGradientWidget::mouseReleaseEvent( QMouseEvent* e )
{
	if( e->button() == Qt::RightButton )
	{
		if( ( e->y() > height() - 14 ) && ( e->y() < height() - 2 ) && ( e->x() > 2 ) && ( e->x() < width() - 1 ) && ( currentPoint % 2 == 1 ) )
		{
			( *m_lpgradient )->m_colorStops.remove( currentPoint / 2 );
			update();
			emit changed();
		}
	}
} // VGradientWidget::mouseReleaseEvent

void VGradientWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
	if( ( e->y() > height() - 14 ) && ( e->y() < height() - 2 ) && ( e->x() > 2 ) && ( e->x() < width() - 1 ) )
	{
		if( currentPoint % 2 == 1 )
		{
			VColorDlg* d = new VColorDlg( ( *m_lpgradient )->m_colorStops.at( currentPoint / 2 )->color, this->topLevelWidget() );
			if( d->exec() == QDialog::Accepted )
			{
				( *m_lpgradient )->m_colorStops.at( currentPoint / 2 )->color = d->Color();
				update();
				emit changed();
			}
			delete d;
		}
		else if( currentPoint == 0 )
		{
			VColorDlg* d = new VColorDlg( ( *m_lpgradient )->m_colorStops.at( 0 )->color, this->topLevelWidget() );
			if( d->exec() == QDialog::Accepted )
			{
				( *m_lpgradient )->addStop( d->Color(), (float)( e->x() - 2 ) / ( width() - 4 ), 0.5 );
				update();
				emit changed();
			}
			delete d;
		}
	}
} // VGradientWidget::mouseDoubleClickEvent

void VGradientWidget::mouseMoveEvent( QMouseEvent* e )
{
	QPtrList<VColorStop>& colorStops = ( *m_lpgradient )->m_colorStops;

	if( ( e->y() > height() - 14 ) && ( e->y() < height() - 2 ) && ( e->x() > 2 ) && ( e->x() < width() - 1 ) )
	{
		if( currentPoint < colorStops.count())
		{
			if( currentPoint % 2 == 1 )
			{
				int x = e->x();
				// Clip the color stop between to others.
				x = kMin( x, ( currentPoint < ( colorStops.count() - 1 ) * 2 ? int( colorStops.at( currentPoint / 2 + 1 )->rampPoint * ( width() - 4 ) + 2 ) : width() - 3 ) );
				x = kMax( x, ( currentPoint > 1 ? int( colorStops.at( currentPoint / 2 - 1 )->rampPoint * ( width() - 4 ) ) + 2 : 2 ) );
				colorStops.at( currentPoint / 2 )->rampPoint = (float)( x - 2 ) / ( width() - 4 );
				update();
				emit changed();
			}
			else if( currentPoint > 0 )
			{
				int x = e->x();
				// Clip the mid point between to ramp points.
				x = kMin( x, int( colorStops.at( currentPoint / 2 )->rampPoint * ( width() - 4 ) + 2 ) );
				x = kMax( x, int( colorStops.at( currentPoint / 2 - 1 )->rampPoint * ( width() - 4 ) + 2 ) );
				colorStops.at( currentPoint / 2 - 1 )->midPoint = ( ( (float)( x-2 ) ) / ( width() - 4 ) - ( colorStops.at( currentPoint / 2 - 1 )->rampPoint ) ) / ( colorStops.at( currentPoint / 2 )->rampPoint - colorStops.at( currentPoint / 2 - 1 )->rampPoint );
				update();
				emit changed();
			}
		}
	}
} // VGradientWidget::mouseMoveEvent

#include "vgradientwidget.moc"
