/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#include <qwmatrix.h>

#include "vglobal.h"
#include "vpolygon.h"
#include "vtransformcmd.h"


VPolygon::VPolygon( VObject* parent,
		const KoPoint& center, double radius, uint edges, double angle )
	: VComposite( parent )
{
	setDrawCenterNode();

	// A polygon should have at least 3 edges:
	if( edges < 3 )
		edges = 3;

	// Make sure the radius is positive:
	if( radius < 0.0 )
		radius = -radius;


	// we start at m_angle + VGlobal::pi_2:
	KoPoint p(
		radius * cos( angle + VGlobal::pi_2 ),
		radius * sin( angle + VGlobal::pi_2 ) );

	moveTo( p );

	for ( uint i = 0; i < edges - 1; ++i )
	{
		p.setX(
			radius * cos( angle + VGlobal::pi_2
			+ VGlobal::twopi / edges * ( i + 1 ) ) );
		p.setY(
			radius * sin( angle + VGlobal::pi_2
			+ VGlobal::twopi / edges * ( i + 1 ) ) );

		lineTo( p );
	}
	close();

	// translate path to center:
	QWMatrix m;
	m.translate( center.x(), center.y() );

	VTransformCmd cmd( 0L, m );
	cmd.visit( *this );
}

