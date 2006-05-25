/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VKOPAINTER_H__
#define __VKOPAINTER_H__

// kopainter/libart wrapper

#include "vpainter.h"
#include <qmatrix.h>
#include <q3ptrlist.h>
#include <koffice_export.h>
class QPainter;
struct _ArtVpath;
struct _ArtBpath;
struct _ArtSVP;
struct _ArtGradientStop;
class VGradient;
class VPattern;
class KoRect;

class KARBONBASE_EXPORT VKoPainter : public VPainter
{
public:
	VKoPainter( QPaintDevice *target, unsigned int w = 0, unsigned int h = 0, bool = true );
	VKoPainter( unsigned char *buffer, unsigned int w = 0, unsigned int h = 0, bool = true );
	virtual ~VKoPainter();

	virtual void resize( unsigned int w, unsigned int h );
	virtual void begin();
	virtual void end();
	virtual void blit( const KoRect & );
	void clear();
	virtual void clear( const QColor & );
	virtual void clear( const KoRect &, const QColor & );

	// matrix manipulation
	virtual void setMatrix( const QMatrix & );
	virtual const QMatrix worldMatrix() { return m_matrix; }
	virtual void setZoomFactor( double );
	virtual double zoomFactor() { return m_zoomFactor; }

	// drawing
	virtual void moveTo( const KoPoint & );
	virtual void lineTo( const KoPoint & );
	virtual void curveTo( const KoPoint &, const KoPoint &, const KoPoint & );
	virtual void newPath();
	virtual void fillPath();
	virtual void setFillRule( VFillRule );
	virtual void strokePath();
	virtual void setClipPath();
	virtual void resetClipPath();

	// helper
	virtual void drawNode( const KoPoint& p, int width );
	virtual void drawRect( const KoRect & );
	virtual void drawRect( double, double, double, double );

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

	//
	/* Port to Qt4
	virtual void setRasterOp( Qt::RasterOp );
	*/

	virtual QPaintDevice *device() { return m_target; } 
	unsigned char *buffer() { return m_buffer; }

private:
	void drawVPath( struct _ArtVpath * );
	void applyGradient( _ArtSVP *, bool );
	void applyPattern( _ArtSVP *, bool );
	_ArtGradientStop *buildStopArray( VGradient &gradient, int & );
	void clampToViewport( const _ArtSVP &, int &, int &, int &, int & );
	void clampToViewport( int &, int &, int &, int & );
	void ensureSpace( unsigned int );

private:
	struct _ArtBpath *m_path;
	unsigned int m_index;
	unsigned int m_alloccount;
	unsigned char *m_buffer;
	QPaintDevice *m_target;
	unsigned int m_width;
	unsigned int m_height;
	QMatrix m_matrix;
	VStroke *m_stroke;
	VFill *m_fill;
	VFillRule m_fillRule;
	double m_zoomFactor;
	Q3PtrList<_ArtSVP> m_clipPaths;

	bool m_bDrawNodes;

	GC gc;
};

#endif
