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

#include <klocale.h>

#include "vcomposite.h"
#include "vglobal.h"
//#include "vinsertknots.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"
#include "vwhirlpinchcmd.h"


VWhirlPinchCmd::VWhirlPinchCmd( VDocument* doc,
	double angle, double pinch, double radius )
		: VCommand( doc, i18n( "Whirl Pinch" ) )
{
	m_selection = document()->selection()->clone();

	m_angle = angle;
	m_pinch = pinch;
	m_radius = radius;
	m_center = m_selection->boundingBox().center();
}

VWhirlPinchCmd::~VWhirlPinchCmd()
{
	delete( m_selection );
}

void
VWhirlPinchCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
		visit( *itr.current() );
}

void
VWhirlPinchCmd::unexecute()
{
}

void
VWhirlPinchCmd::visitVComposite( VComposite& composite )
{
	// first subdivide:
//	VInsertKnots insertKnots( 2 );
//	insertKnots.visit( composite );

	VVisitor::visitVComposite( composite );
}

void
VWhirlPinchCmd::visitVPath( VPath& path )
{
	QWMatrix m;
	KoPoint delta;
	double dist;

	path.first();

	while( path.current() )
	{
// TODO: selfmade this function since it's gone:
//		path.current()->convertToCurve();


		// Apply three times separately to each segment node.

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


		delta = path.current()->point( 0 ) - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

		if( dist < m_radius )
		{
			m.reset();

			dist /= m_radius;

			// Pinch.
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// Whirl.
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );
			m.translate( m_center.x(), m_center.y() );

			path.current()->setPoint( 0, delta.transform( m ) );
		}


		delta = path.current()->point( 1 ) - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

		if( dist < m_radius )
		{
			m.reset();

			dist /= m_radius;

			// Pinch.
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// Whirl.
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );
			m.translate( m_center.x(), m_center.y() );

			path.current()->setPoint( 1, delta.transform( m ) );
		}


		if( !success() )
			setSuccess();


		path.next();
	}

	// Invalidate bounding box once.
	path.invalidateBoundingBox();
}

