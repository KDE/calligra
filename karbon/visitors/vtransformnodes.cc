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


#include "vpath.h"
#include "vsegment.h"
#include "vtransformnodes.h"


VTransformNodes::VTransformNodes( const QWMatrix& m )
	: m_matrix( m )
{
}

void
VTransformNodes::visitVPath( VPath& path )
{
	path.first();

	// skip "begin":
	while( path.next() )
	{
/*		if(
			path.current()->prev() &&
			path.current()->prev()->edited( 2 ) )
		{
			// Do nothing.
		}
		else if( path.current()->edited( 0 ) )
		{
			path.current()->setPoint(
				0,
				path.current()->point( 0 ).transform( m_matrix ) );
		}

		if( path.current()->edited( 2 ) )
		{
			path.current()->setPoint(
				1,
				path.current()->point( 1 ).transform( m_matrix ) );
			path.current()->setKnot(
				path.current()->knot().transform( m_matrix ) );

			if( path.current() == path.getLast() )
			{
				path.getFirst()->setKnot(
					path.getFirst()->knot().transform( m_matrix ) );
				path.getFirst()->next()->setPoint(
					0,
					path.getFirst()->next()->point( 0 ).transform( m_matrix ) );
			}
			else
			{
				path.current()->next()->setPoint(
					0,
					path.current()->next()->point( 0 ).transform( m_matrix ) );
			}
		}
		else if( path.current()->edited( 1 ) )
		{
			path.current()->setPoint(
				1,
				path.current()->point( 1 ).transform( m_matrix ) );
		}
*/

		if( !success() )
			setSuccess();
	}
}

