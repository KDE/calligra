/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VGRADIENT_H__
#define __VGRADIENT_H__

#include <qvaluelist.h>
#include <koPoint.h>
#include "vcolor.h"


class QDomElement;


class VGradient
{
public:
	enum VGradientType
	{
		linear = 0,
		radial = 1,
		conic  = 2
	};

	enum VGradientRepeatMethod
	{
		none    = 0,
		reflect = 1,
		repeat  = 2
	};

	struct VColorStop
	{
		VColor color;

		// relative position of color point (0.0-1.0):
		float rampPoint;

		// relative position of midpoint (0.0-1.0)
		// between two ramp points. ignored for last VColorStop.
		float midPoint;
	};

	VGradient( VGradientType type = linear );

	VGradientType type() const { return m_type; }
	void setType( VGradientType type ) { m_type = type; }

	VGradientRepeatMethod repeatMethod() const { return m_repeatMethod; }
	void setRepeatMethod( VGradientRepeatMethod repeatMethod ) { m_repeatMethod = repeatMethod; }

	const QValueList<VColorStop>& colorStops() const { return m_colorStops; }
	void addStop( const VColor &color, float rampPoint, float midPoint );
	void clearStops();

	KoPoint origin() const { return m_origin; }
	void setOrigin( const KoPoint &origin ) { m_origin = origin; }

	KoPoint vector() const { return m_vector; }
	void setVector( const KoPoint &vector ) { m_vector = vector; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	void transform( const QWMatrix& m );

private:
	VGradientType m_type;
	VGradientRepeatMethod m_repeatMethod;

	QValueList<VColorStop> m_colorStops;

	// coordinates:
	KoPoint m_origin;
	KoPoint m_vector;
};

#endif
