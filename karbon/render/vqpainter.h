/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VQPAINTER_H__
#define __VQPAINTER_H__

// qpainter wrapper

#include "vpainter.h"

class QPainter;

class VQPainter : public VPainter
{
public:
	VQPainter( QWidget *target, int w = 0, int h = 0 );
	virtual ~VQPainter();

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
	// virtual void setBrush( const VBrush & );
	//virtual void setPen( const QPen & );
	virtual void setPen( const QColor & );
	virtual void setPen( Qt::PenStyle style );
	virtual void setBrush( const QBrush & );
	virtual void setBrush( const QColor & );
	virtual void setBrush( Qt::BrushStyle style );

	// stack management
	virtual void save();
	virtual void restore();

	//
	virtual void setRasterOp( Qt::RasterOp );

private:
	QPainter *m_painter;
};

#endif
