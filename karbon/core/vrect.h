/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VRECT_H__
#define __VRECT_H__

#include <qrect.h>

#include "vpoint.h"

// This class is mostly used for bounding-boxes and intersection calculations.

class VRect
{
public:
	VRect();

	const VPoint& topLeft() const { return m_topLeft; }
	const VPoint& bottomRight() const { return m_bottomRight; }

	const QRect& getQRect( const double& zoomFactor ) const;

private:
	VPoint m_topLeft;
	VPoint m_bottomRight;

	mutable QRect m_QRect;
};

#endif
