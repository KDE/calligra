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
	VQPainter( QPaintDevice *target, int w = 0, int h = 0 );
	virtual ~VQPainter();

    //
	virtual void resize( int w, int h );
	virtual void begin();
	virtual void end();
	virtual void blit( const QRect & ) {}

	// matrix manipulation
	virtual void setWorldMatrix( const QWMatrix & );
	virtual void setZoomFactor( double );

	// drawing
    virtual void moveTo( const KoPoint & );
    virtual void lineTo( const KoPoint & );
	virtual void curveTo( const KoPoint &, const KoPoint &, const KoPoint & );
    virtual void newPath();
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

	virtual QPaintDevice *device() { return m_target; }
private:
	double m_zoomFactor;
	unsigned int m_index;
	QPointArray m_pa;
	QPainter *m_painter;
	QPaintDevice *m_target;
	int m_width;
	int m_height;
};

#endif
