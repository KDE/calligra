/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qwmatrix.h>

#include "vglobal.h"
#include "vpath.h"
#include "vsegment.h"
#include "vwhirlpinch.h"


void
VWhirlPinch::visitVPath(
	VPath& /*path*/, QPtrList<VSegmentList>& lists )
{
kdDebug() << "*** r: " << m_radius << endl;
	QWMatrix m;
	KoPoint delta;
	double dist;

	QPtrListIterator<VSegmentList> itr( lists );
	for( ; itr.current(); ++itr )
	{
		itr.current()->first();

		while( itr.current()->current() )
		{
			itr.current()->current()->convertToCurve();


			delta = itr.current()->current()->knot2() - m_center;
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

				itr.current()->current()->setKnot2( delta.transform( m ) );
			}


			if( itr.current()->current()->type() == segment_begin )
			{
				itr.current()->next();
				continue;
			}


			delta = itr.current()->current()->ctrlPoint1() - m_center;
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

				itr.current()->current()->setCtrlPoint1( delta.transform( m ) );
			}


			delta = itr.current()->current()->ctrlPoint2() - m_center;
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

				itr.current()->current()->setCtrlPoint2( delta.transform( m ) );
			}


			itr.current()->next();
		}
	}
}

