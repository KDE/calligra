/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VWHIRLPINCH_H__
#define __VWHIRLPINCH_H__

#include <koPoint.h>

#include "vpath.h"
#include "vvisitor.h"


class VWhirlPinch : public VVisitor
{
public:
	VWhirlPinch( const KoPoint& center, double angle, double pinch, double radius )
		: m_center( center )
	{
		m_angle = angle;
		m_pinch = pinch;
		m_radius = radius;
	}

	const KoPoint& center() const { return m_center; }
	void setCenter( const KoPoint& center ) { m_center = center; }

	double angle() const { return m_angle; }
	void setAngle( double angle ) { m_angle = angle; }

	double pinch() const { return m_pinch; }
	void setPinch( double pinch ) { m_pinch = pinch; }

	double radius() const { return m_radius; }
	void setRadius( double radius ) { m_radius = radius; }

	virtual void visitVPath(
		VPath& path, QPtrList<VSegmentList>& lists );

private:
	KoPoint m_center;
	double m_angle;
	double m_pinch;
	double m_radius;
};

#endif

