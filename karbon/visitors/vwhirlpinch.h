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

#ifndef __VWHIRLPINCH_H__
#define __VWHIRLPINCH_H__


#include <koPoint.h>

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

	virtual ~VWhirlPinch() {}

	const KoPoint& center() const { return m_center; }
	void setCenter( const KoPoint& center ) { m_center = center; }

	double angle() const { return m_angle; }
	void setAngle( double angle ) { m_angle = angle; }

	double pinch() const { return m_pinch; }
	void setPinch( double pinch ) { m_pinch = pinch; }

	double radius() const { return m_radius; }
	void setRadius( double radius ) { m_radius = radius; }

	virtual void visitVComposite( VComposite& composite );
	virtual void visitVPath( VPath& path );

private:
	KoPoint m_center;
	double m_angle;
	double m_pinch;
	double m_radius;
};

#endif

