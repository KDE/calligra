/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGRADIENT_H__
#define __VGRADIENT_H__

#include <qvaluelist.h>
#include "vcolor.h"


enum VGradientType{
	gradient_linear = 0,
	gradient_radial = 1,
	gradient_conic  = 2 };


class QDomElement;

class VGradient
{
public:
	struct VColorStop
	{
		VColor color;

		// relative position of color point (0.0-1.0):
		float rampPoint;

		// relative position of midpoint (0.0-1.0)
		// between two ramp points. ignored for last VColorStop.
		float midPoint;
	};

	VGradient( VGradientType type = gradient_linear );

	VGradientType type() const { return m_type; }
	void setType( VGradientType type ) { m_type = type; }

	QValueList<VColorStop>& colorStops() { return m_colorStops; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VGradientType m_type;

	QValueList<VColorStop> m_colorStops;
};

#endif
