/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VKOPAINTER_H__
#define __VKOPAINTER_H__

// kopainter/libart wrapper

#include "vpainter.h"
#include <qwmatrix.h>

#include <art_misc.h>
#include <art_vpath.h>

class QPainter;

class VKoPainter : public VPainter
{
public:
	VKoPainter( QWidget *target, int w = 0, int h = 0 );
	virtual ~VKoPainter();

    //
	virtual void resize( int w, int h );
	virtual void begin();
	virtual void end();

	// matrix manipulation
	virtual void setWorldMatrix( const QWMatrix & );

	// drawing
	virtual void drawPolygon( const QPointArray &, bool winding = false );
	virtual void drawPolyline( const QPointArray & );
	virtual void drawRect( double x, double y, double w, double h );

	// pen + brush
	virtual void setPen( const VStroke & );
	virtual void setPen( const QColor & );
	virtual void setPen( Qt::PenStyle style );
	virtual void setBrush( const VFill & );
	virtual void setBrush( const QColor & );
	virtual void setBrush( Qt::BrushStyle style );

	// stack management
	virtual void save();
	virtual void restore();

	//
	virtual void setRasterOp( Qt::RasterOp );
private:
	void clear();
	void clear( unsigned int color );
	void drawVPath( ArtVpath * );

private:
	art_u8 *m_buffer;
	QWidget *m_target;
	unsigned int m_width;
	unsigned int m_height;
	QWMatrix m_matrix;
	VStroke *m_stroke;
	VFill *m_fill;

	GC gc;
};

#endif
