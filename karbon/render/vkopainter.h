/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VKOPAINTER_H__
#define __VKOPAINTER_H__

// kopainter/libart wrapper

#include "vpainter.h"
#include <qwmatrix.h>

class QPainter;
struct _ArtVpath;
struct _ArtBpath;
struct _ArtSVP;
struct _ArtGradientStop;
class VGradient;

class VKoPainter : public VPainter
{
public:
	VKoPainter( QPaintDevice *target, unsigned int w = 0, unsigned int h = 0 );
	virtual ~VKoPainter();

    //
	virtual void resize( unsigned int w, unsigned int h );
	virtual void begin();
	virtual void end();
	void clear();
	void clear( unsigned int color );
	virtual void blit( const QRect & );

	// matrix manipulation
	virtual void setWorldMatrix( const QWMatrix & );
	virtual void setZoomFactor( double );
	virtual double zoomFactor() { return m_zoomFactor; }

	// drawing
    virtual void moveTo( const KoPoint & );
    virtual void lineTo( const KoPoint & );
    virtual void curveTo( const KoPoint &, const KoPoint &, const KoPoint & );
    virtual void newPath();
    virtual void fillPath();
    virtual void strokePath();

	// pen + brush
	virtual void setPen( const VStroke & );
	virtual void setPen( const QColor & );
	virtual void setPen( Qt::PenStyle style );
	virtual void setBrush( const VFill & );
	virtual void setBrush( const QColor & );
	virtual void setBrush( Qt::BrushStyle style );

	virtual void drawImage( const QImage & );

	// stack management
	virtual void save();
	virtual void restore();

	//
	virtual void setRasterOp( Qt::RasterOp );

	virtual QPaintDevice *device() { return m_target; } 

private:
	void drawVPath( struct _ArtVpath * );
	void applyGradient( _ArtSVP *, bool );
	_ArtGradientStop *buildStopArray( VGradient &gradient, int & );
	void clampToViewport( const _ArtSVP &, int &, int &, int &, int & );

private:
	struct _ArtBpath *m_path;
	unsigned int m_index;
	unsigned char *m_buffer;
	QPaintDevice *m_target;
	unsigned int m_width;
	unsigned int m_height;
	QWMatrix m_matrix;
	VStroke *m_stroke;
	VFill *m_fill;
	double m_zoomFactor;

	GC gc;
};

#endif
