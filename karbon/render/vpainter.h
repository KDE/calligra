/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPAINTER_H__
#define __VPAINTER_H__

// painter abstraction

#include <qnamespace.h>
#include <koRect.h>

class QWMatrix;
class QPaintDevice;
class QColor;
class QPen;
class QBrush;
class QRect;
class QImage;

class VStroke;
class VFill;

class KoPoint;

class VPainter
{
public:
	VPainter( QPaintDevice *, unsigned int /*w*/ = 0, unsigned int /*h*/ = 0 ) {};
	virtual ~VPainter() {};

	//
	virtual void resize( unsigned int w, unsigned int h ) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void blit( const QRect & ) = 0;

	// matrix manipulation
	virtual void setWorldMatrix( const QWMatrix & ) = 0;
	virtual void setZoomFactor( double ) = 0;
	virtual double zoomFactor() { return 1.0; }

	// drawing
	virtual void moveTo( const KoPoint & ) = 0;
	virtual void lineTo( const KoPoint & ) = 0;
	virtual void curveTo( const KoPoint &, const KoPoint &, const KoPoint & ) = 0;
	virtual void newPath() = 0;
	virtual void strokePath() = 0;
	virtual void fillPath() = 0;

	// pen + brush
	virtual void setPen( const VStroke & ) = 0;
	// compatibility, use VPen/VBrush later ?
	virtual void setPen( const QColor & ) = 0;
	virtual void setPen( Qt::PenStyle style ) = 0;
	virtual void setBrush( const VFill & ) = 0;
	virtual void setBrush( const QColor & ) = 0;
	virtual void setBrush( Qt::BrushStyle style ) = 0;

	virtual void drawImage( const QImage & ) {}

	// stack management
	virtual void save() = 0;
	virtual void restore() = 0;

	// we have to see how this fits in
	virtual void setRasterOp( Qt::RasterOp ) = 0;

	// access to device
	virtual QPaintDevice *device() = 0;

	virtual KoRect boundingBox() const { return KoRect(); }
};

#endif
