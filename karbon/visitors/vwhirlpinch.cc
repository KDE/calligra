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


#include <math.h>

#include <qptrlist.h>
#include <qwmatrix.h>

#include "vcomposite.h"
#include "vglobal.h"
#include "vinsertknots.h"
#include "vpath.h"
#include "vsegment.h"
#include "vwhirlpinch.h"


void
VWhirlPinch::visitVComposite( VComposite& composite )
{
	// first subdivide:
	VInsertKnots insertKnots( 2 );
	insertKnots.visit( composite );

	VVisitor::visitVComposite( composite );
}

void
VWhirlPinch::visitVPath( VPath& path )
{
	QWMatrix m;
	KoPoint delta;
	double dist;

	path.first();

	while( path.current() )
	{
		path.current()->convertToCurve();


		delta = path.current()->knot() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

		if( dist < m_radius )
		{
			m.reset();

			dist /= m_radius;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );
			m.translate( m_center.x(), m_center.y() );

			path.current()->setKnot( delta.transform( m ) );
		}


		delta = path.current()->ctrlPoint1() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

		if( dist < m_radius )
		{
			m.reset();

			dist /= m_radius;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );
			m.translate( m_center.x(), m_center.y() );

			path.current()->setCtrlPoint1( delta.transform( m ) );
		}


		delta = path.current()->ctrlPoint2() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

		if( dist < m_radius )
		{
			m.reset();

			dist /= m_radius;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );
			m.translate( m_center.x(), m_center.y() );

			path.current()->setCtrlPoint2( delta.transform( m ) );
		}


		if( !success() )
			setSuccess();


		path.next();
	}

	// invalidate bounding box once:
	path.invalidateBoundingBox();
}

