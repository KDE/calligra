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

#ifndef __VQPAINTER_H__
#define __VQPAINTER_H__

// qpainter wrapper

#include "vpainter.h"

#include <QPainterPath>
#include <QRectF>
#include <QPointF>

class QPainter;

class VQPainter : public VPainter
{
public:
	VQPainter( QPaintDevice *target, unsigned int w = 0, unsigned int h = 0, bool drawNodes = true );
	VQPainter( unsigned char *buffer, unsigned int w = 0, unsigned int h = 0, bool drawNodes = true );
	virtual ~VQPainter();

	//
	virtual void resize( unsigned int w, unsigned int h );
	virtual void begin();
	virtual void end();
	virtual void blit( const QRectF & );
	virtual void clear( const QColor & );
	virtual void clear( const QRectF &, const QColor & ) {}

	// matrix manipulation
	virtual void setMatrix( const QMatrix & );
	virtual const QMatrix worldMatrix();
	virtual void setZoomFactor( double );
	virtual double zoomFactor() { return m_zoomFactor; }

	// drawing
	virtual void moveTo( const QPointF & );
	virtual void lineTo( const QPointF & );
	virtual void curveTo( const QPointF &, const QPointF &, const QPointF & );
	virtual void newPath();
	virtual void strokePath();
	virtual void fillPath();
	virtual void setFillRule( VFillRule ) {}
	virtual void setClipPath() {}
	virtual void resetClipPath() {}

	// pen + brush
	virtual void setPen( const VStroke & );
	virtual void setPen( const QColor & );
	virtual void setPen( Qt::PenStyle style );
	virtual void setBrush( const VFill & );
	virtual void setBrush( const QColor & );
	virtual void setBrush( Qt::BrushStyle style );

	virtual void drawImage( const QImage &, const QMatrix & );

	// stack management
	virtual void save();
	virtual void restore();

	// helper
	virtual void drawNode( const QPointF& , int );
	virtual void drawRect( const QRectF & );
	virtual void drawRect( double, double, double, double );

	//
	/* Port to Qt4
	virtual void setRasterOp( Qt::RasterOp );
	*/

	virtual QPaintDevice *device() { return m_target; }

private:
	double m_zoomFactor;
	int m_index;
	QPainterPath m_pa;
	QPainter *m_painter;
	QPaintDevice *m_target;
	unsigned int m_width;
	unsigned int m_height;
	unsigned char *m_buffer;
	bool m_bDrawNodes;
};

#endif
