/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGRADIENT_H__
#define __VGRADIENT_H__

#include <qvaluelist.h>
#include "vcolor.h"

class QDomElement;

class VGradient
{
public:
	enum VGradientType { linear, radial };

	struct VColorPoint
	{
		VColor color;
		float position;
	};

	VGradient( VGradientType type = linear );

	VGradientType type() const { return m_type; }
	void setType( VGradientType type ) { m_type = type; }

	const QValueList<float>& midPoints() const
		{ return m_midPoints; }
	void setMidPoints( const QValueList<float>& points )
		{ m_midPoints = points; }

	const QValueList<VColorPoint>& colorPoints() const
		{ return m_colorPoints; }
	void setColorPoints( const QValueList<VColorPoint>& points )
		{ m_colorPoints = points; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VGradientType m_type;

	QValueList<float> m_midPoints;
	QValueList<VColorPoint> m_colorPoints;
};

#endif
