/* This file is part of the KDE project
   Copyright (C) 2002 - 2005, The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VGRADIENT_H__
#define __VGRADIENT_H__

#include <q3ptrlist.h>
#include <q3ptrvector.h>

#include <koffice_export.h>
#include <QPointF>

#include "vcolor.h"

class QMatrix;
class QDomElement;
class KoGenStyle;
class KoGenStyles;
class KoStyleStack;
class VObject;

class VColorStop
{
	public:
		VColorStop( double r, double m, VColor c )
			{ rampPoint = r; midPoint = m; color = c; };
		VColorStop( const VColorStop& colorStop )
			{ rampPoint = colorStop.rampPoint; midPoint = colorStop.midPoint; color = colorStop.color; };

		VColor color;

		// relative position of color point (0.0-1.0):
		float rampPoint;

		// relative position of midpoint (0.0-1.0)
		// between two ramp points. ignored for last VColorStop.
		float midPoint;

		friend inline bool operator== ( VColorStop& s1, VColorStop& s2 )
				{ return s1.rampPoint == s2.rampPoint; };
}; // VColorStop

class KARBONBASE_EXPORT VGradient
{
friend class VGradientWidget;

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

	class VColorStopList : public Q3PtrList<VColorStop>
	{
		protected:
			virtual int compareItems( Q3PtrCollection::Item item1, Q3PtrCollection::Item item2 );
	}; // VColorStopList

	VGradient( VGradientType type = linear );
	VGradient( const VGradient& gradient );

	VGradient& operator=(const VGradient& gradient);

	VGradientType type() const { return m_type; }
	void setType( VGradientType type ) { m_type = type; }

	VGradientRepeatMethod repeatMethod() const { return m_repeatMethod; }
	void setRepeatMethod( VGradientRepeatMethod repeatMethod ) { m_repeatMethod = repeatMethod; }

	const Q3PtrVector<VColorStop> colorStops() const;
	void addStop( const VColorStop& colorStop );
	void addStop( const VColor &color, float rampPoint, float midPoint );
	void removeStop( const VColorStop& colorStop );
	void clearStops();

	QPointF origin() const { return m_origin; }
	void setOrigin( const QPointF &origin ) { m_origin = origin; }

	QPointF focalPoint() const { return m_focalPoint; }
	void setFocalPoint( const QPointF &focalPoint ) { m_focalPoint = focalPoint; }

	QPointF vector() const { return m_vector; }
	void setVector( const QPointF &vector ) { m_vector = vector; }

	void save( QDomElement& element ) const;
	QString saveOasis( KoGenStyles &mainStyles ) const;
	void load( const QDomElement& element );
	void loadOasis( const QDomElement &object, KoStyleStack &stack, VObject* parent = 0L );

	void transform( const QMatrix& m );

protected:
	VColorStopList        m_colorStops;
  
private:
	VGradientType         m_type		: 2;
	VGradientRepeatMethod m_repeatMethod	: 2;

	// coordinates:
	QPointF m_origin;
	QPointF m_focalPoint;
	QPointF m_vector;
}; // VGradient

#endif /* __VGRADIENT_H__ */
