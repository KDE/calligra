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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <qptrlist.h>
#include <qvaluelist.h>

#include <klocale.h>

#include "vbooleancmd.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"
#include "vdocument.h"

VBooleanCmd::VBooleanCmd( VDocument *doc, VBooleanType type )
	: VCommand( doc, i18n( "Boolean Operation" ) )
{
	m_selection = document()->selection()->clone();
	m_type = type;
}

VBooleanCmd::~VBooleanCmd()
{
	delete( m_selection );
}

void
VBooleanCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
// TODO: pair wise visiting.
	}

//	document()->append(  );
	document()->selection()->clear();
}

void
VBooleanCmd::unexecute()
{
}

bool
VBooleanCmd::visit( VObject& object1, VObject& object2 )
{
	m_path1 = 0L;
	m_path2 = 0L;
	object1.accept( *this );
	object2.accept( *this );

	return success();
}

void
VBooleanCmd::visitVSubpath( VSubpath& path )
{
	if( m_path1 == 0L )
		m_path1 = &path;
	else if( m_path2 == 0L )
	{
		m_path2 = &path;

		// intersection parameters (t):
		VParamList params1;
		VParamList params2;
		VParamList::iterator pItr;

		double prevParam;

		m_path1->first();

		// ommit "begin" segment:
		while( m_path1->next() )
		{
			params1.clear();

			m_path2->first();

			// ommit "begin" segment:
			while( m_path2->next() )
			{
				params2.clear();
			
				recursiveSubdivision(
					*m_path1->current(), 0.0, 1.0,
					*m_path2->current(), 0.0, 1.0,
					params1, params2 );

				qHeapSort( params2 );

				prevParam = 0.0;

				// walk down all intersection params and insert knots:
				for( pItr = params2.begin(); pItr != params2.end(); ++pItr )
				{
					m_path2->insert(
						m_path2->current()->splitAt(
							( *pItr - prevParam )/( 1.0 - prevParam ) ) );

					m_path2->next();
					prevParam = *pItr;
				}
			}

			qHeapSort( params1 );

			prevParam = 0.0;

			// walk down all intersection params and insert knots:
			for( pItr = params1.begin(); pItr != params1.end(); ++pItr )
			{
				m_path1->insert(
					m_path1->current()->splitAt(
						( *pItr - prevParam )/( 1.0 - prevParam ) ) );

				m_path1->next();
				prevParam = *pItr;
			}
		}
	}
}

void
VBooleanCmd::recursiveSubdivision(
	const VSegment& segment1, double t0_1, double t1_1,
	const VSegment& segment2, double t0_2, double t1_2,
	VParamList& params1, VParamList& params2 )
{
	if(
		!segment1.boundingBox().intersects(
			segment2.boundingBox() ) )
	{
		return;
	}

	if( segment1.isFlat() )
	{
		// calculate intersection of line segments:
		if( segment2.isFlat() )
		{
			KoPoint v1  = segment1.knot() - segment1.prev()->knot();
			KoPoint v2  = segment2.knot() - segment2.prev()->knot();

			double det = v2.x() * v1.y() - v2.y() * v1.x();

			if( 1.0 + det == 1.0 )
				return;
			else
			{
				KoPoint v = segment2.prev()->knot() - segment1.prev()->knot();
				const double one_det = 1.0 / det;

				double t1 = one_det * ( v2.x() * v.y() - v2.y() * v.x() );
				double t2 = one_det * ( v1.x() * v.y() - v1.y() * v.x() );

				if ( t1 < 0.0 || t1 > 1.0 || t2 < 0.0 || t2 > 1.0 )
					return;

				params1.append( t0_1 + t1 * ( t1_1 - t0_1 ) );
				params2.append( t0_2 + t2 * ( t1_2 - t0_2 ) );
			}
		}
		else
		{
			// "copy segment" and split it at midpoint:
			VSubpath path2( segment2 );
			path2.insert( path2.current()->splitAt( 0.5 ) );

			double mid2 = 0.5 * ( t0_2 + t1_2 );

			recursiveSubdivision(
				*path2.current(), t0_2, mid2,
				segment1,         t0_1, t1_1, params2, params1 );
			recursiveSubdivision(
				*path2.next(),    mid2, t1_2,
				segment1,         t0_1, t1_1, params2, params1 );
		}
	}
	else
	{
		// "copy segment" and split it at midpoint:
		VSubpath path1( segment1 );
		path1.insert( path1.current()->splitAt( 0.5 ) );

		double mid1 = 0.5 * ( t0_1 + t1_1 );

		if( segment2.isFlat() )
		{
			recursiveSubdivision(
				*path1.current(), t0_1, mid1,
				segment2,         t0_2, t1_2, params1, params2 );
			recursiveSubdivision(
				*path1.next(),    mid1, t1_1,
				segment2,         t0_2, t1_2, params1, params2 );
		}
		else
		{
			// "copy segment" and split it at midpoint:
			VSubpath path2( segment2 );
			path2.insert( path2.current()->splitAt( 0.5 ) );

			double mid2 = 0.5 * ( t0_2 + t1_2 );

			recursiveSubdivision(
				*path1.current(), t0_1, mid1,
				*path2.current(), t0_2, mid2, params1, params2 );
			recursiveSubdivision(
				*path1.next(),    mid1, t1_1,
				*path2.current(), t0_2, mid2, params1, params2 );

			recursiveSubdivision(
				*path1.prev(),    t0_1, mid1,
				*path2.next(),    mid2, t1_2, params1, params2 );
			recursiveSubdivision(
				*path1.next(),    mid1, t1_1,
				*path2.current(), mid2, t1_2, params1, params2 );
		}
	}
}

