/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPOINT_H__
#define __VPOINT_H__

#include <qpoint.h>

/**
 * A VPoint acts like a vector and is used to describe coordinates.
 * It includes a QPoint for direct use in painting on a QPainter.
 * Before 
 */

class VPoint
{
public:
	VPoint();
	VPoint( const VPoint& p );
	VPoint( const double& x, const double& y );

	// convert to QPoint and recalculate if necessary:
	const QPoint& getQPoint( const double& zoomFactor ) const;

	void moveTo( const double& x, const double& y );
	void rmoveTo( const double& x, const double& y );

	const double& x() const { return m_x; }
	void setX( const double& x );
	const double& y() const { return m_y; }
	void setY( const double& y );

//    VPoint& operator= (const VPoint& p) { return *this; }
	void operator +=( const VPoint& p ) { m_x += p.m_x; m_y += p.m_y; }
	void operator -=( const VPoint& p ) { m_x -= p.m_x; m_y -= p.m_y; }

	// ref-counting (vpaths share vpoints):
	unsigned int ref()   { return ++m_refCount; }
	unsigned int unref() { return --m_refCount; }

	/**
	* we scale QPoint with fractScale, i.e. we consider fractBits bits
	* of the fraction of each double-coordinate.
	*/
	static const char s_fractBits = 12;
	static const unsigned int s_fractScale = 1 << s_fractBits;
	static const double s_fractInvScale; // = 1/s_fractScale

private:
	double m_x;
	double m_y;
	mutable QPoint m_QPoint;	// for painting

	unsigned int m_refCount;	// how many objects use this vpoint ?
};

inline bool operator ==( const VPoint& l, const VPoint& r )
	{ return ( l.x()==r.x() && l.y()==r.y() ); }

inline bool operator !=( const VPoint& l, const VPoint& r )
	{ return !operator==( l, r ); }

#endif
