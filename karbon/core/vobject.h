/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include "vrect.h"

class QPainter;

class VAffineMap;

// The base class for all karbon objects.

class VObject
{
public:
	VObject();
	virtual ~VObject() {}

	virtual void draw( QPainter& painter, const QRect& rect,
		const double& zoomFactor ) = 0;

	virtual void translate( const double& dx, const double& dy ) = 0;
	virtual void rotate( const double& ang ) = 0;
	virtual void mirror( const bool horiz = false, const bool verti = false ) = 0;
	virtual void scale( const double& sx, const double& sy ) = 0;
	virtual void shear( const double& sh, const double& sv ) = 0;
	virtual void skew( const double& ang ) = 0;
	virtual void apply( const VAffineMap& affmap ) = 0;

	const VRect& boundingBox() const { return m_boundingBox; }

protected:
	// QRect as boundingBox is sufficent since it's not used for calculating
	// intersections
	VRect m_boundingBox;
};

#endif
