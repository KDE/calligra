/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VQPAINTER_H__
#define __VQPAINTER_H__

// qpainter wrapper

#include "vpainter.h"
#include "qpointarray.h"

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
	virtual void setZoomFactor( double );

	// drawing
	virtual void drawRect( double x, double y, double w, double h );
    virtual void moveTo( const KoPoint & );
    virtual void lineTo( const KoPoint & );
	virtual void curveTo( const KoPoint &, const KoPoint &, const KoPoint & );
    virtual void strokePath();
    virtual void fillPath();

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
	double m_zoomFactor;
	unsigned int m_index;
	QPointArray m_pa;
	QPainter *m_painter;
	QWidget  *m_target;
	int m_width;
	int m_height;
};

#endif
