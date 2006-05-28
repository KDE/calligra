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

#ifndef __VPAINTER_H__
#define __VPAINTER_H__

// painter abstraction

#include <qnamespace.h>
#include <QRectF>
#include <vfillrule.h>

class QMatrix;
class QPaintDevice;
class QColor;
class QPen;
class QBrush;
class QImage;

class VStroke;
class VFill;

class QPointF;

class VPainter
{
public:
	VPainter( QPaintDevice *, unsigned int /*w*/ = 0, unsigned int /*h*/ = 0 ) {};
	virtual ~VPainter() {};

	//
	virtual void resize( unsigned int w, unsigned int h ) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void blit( const QRectF & ) = 0;
	virtual void clear( const QColor & ) = 0;
	virtual void clear( const QRectF &, const QColor & ) = 0;

	// matrix manipulation
	virtual void setMatrix( const QMatrix & ) = 0;
	virtual const QMatrix worldMatrix() = 0;
	virtual void setZoomFactor( double ) = 0;
	virtual double zoomFactor() { return 1.0; }

	// drawing
	virtual void moveTo( const QPointF & ) = 0;
	virtual void lineTo( const QPointF & ) = 0;
	virtual void curveTo( const QPointF &, const QPointF &, const QPointF & ) = 0;
	virtual void newPath() = 0;
	virtual void strokePath() = 0;
	virtual void fillPath() = 0;
	virtual void setFillRule( VFillRule ) = 0;
	virtual void setClipPath() = 0;
	virtual void resetClipPath() = 0;

	// helper
	virtual void drawNode( const QPointF& , int ) {}
	virtual void drawRect( const QRectF & ) {}
	virtual void drawRect( double, double, double, double ) {}

	// pen + brush
	virtual void setPen( const VStroke & ) = 0;
	// compatibility, use VPen/VBrush later ?
	virtual void setPen( const QColor & ) = 0;
	virtual void setPen( Qt::PenStyle style ) = 0;
	virtual void setBrush( const VFill & ) = 0;
	virtual void setBrush( const QColor & ) = 0;
	virtual void setBrush( Qt::BrushStyle style ) = 0;

	virtual void drawImage( const QImage &, const QMatrix & ) {}

	// stack management
	virtual void save() = 0;
	virtual void restore() = 0;

	// we have to see how this fits in
	/* Port to Qt4
	virtual void setRasterOp( Qt::RasterOp ) = 0;
	*/

	// access to device
	virtual QPaintDevice *device() = 0;
};

#endif
