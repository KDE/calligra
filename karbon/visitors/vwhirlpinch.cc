/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qptrlist.h>
#include <qwmatrix.h>

#include "vglobal.h"
#include "vpath.h"
#include "vsegment.h"
#include "vsegmentlist.h"
#include "vwhirlpinch.h"


void
VWhirlPinch::visitVPath( VPath& path )
{
	QPtrListIterator<VSegmentList> itr( path.segmentLists() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VWhirlPinch::visitVSegmentList( VSegmentList& segmentList )
{
kdDebug() << "*** r: " << m_radius << endl;
	QWMatrix m;
	KoPoint delta;
	double dist;

	segmentList.first();

	while( segmentList.current() )
	{
		segmentList.current()->convertToCurve();


		delta = segmentList.current()->knot2() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

kdDebug() << "**" << dist << endl;
		if( dist < m_radius )
		{
			m.reset();

			if( dist != 0.0 )
				dist = m_radius / dist;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );

			m.translate( m_center.x(), m_center.y() );

			segmentList.current()->setKnot2( delta.transform( m ) );
		}


		if( segmentList.current()->type() == segment_begin )
		{
			segmentList.next();
			continue;
		}


		delta = segmentList.current()->ctrlPoint1() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

kdDebug() << "**" << dist << endl;
		if( dist < m_radius )
		{
			m.reset();

			if( dist != 0.0 )
				dist = m_radius / dist;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );

			m.translate( m_center.x(), m_center.y() );

			segmentList.current()->setCtrlPoint1( delta.transform( m ) );
		}


		delta = segmentList.current()->ctrlPoint2() - m_center;
		dist = sqrt( delta.x() * delta.x() + delta.y() * delta.y() );

kdDebug() << "**" << dist << endl;
		if( dist < m_radius )
		{
			m.reset();

			if( dist != 0.0 )
				dist = m_radius / dist;

			// pinch:
			m.scale(
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ),
				pow( sin( VGlobal::pi_2 * dist ), -m_pinch ) );

			// whirl:
			m.rotate( m_angle * ( 1.0 - dist ) * ( 1.0 - dist ) );

			m.translate( m_center.x(), m_center.y() );

			segmentList.current()->setCtrlPoint2( delta.transform( m ) );
		}

		// invalidate bounding box once:
		segmentList.invalidateBoundingBox();

		segmentList.next();
	}
}

