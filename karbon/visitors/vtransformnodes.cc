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
	while( path.current() )
	{
		if( path.current()->knotIsSelected() )
			path.current()->transform( m_matrix );
		else if( path.current()->type() == VSegment::curve )
		{
			//if( path.current()->pointIsSelected( 1 ) )
			//	path.current()->transform( QWMatrix( m_matrix.m11(), m_matrix.m12(), m_matrix.m21(), m_matrix.m22(), -m_matrix.dx(), -m_matrix.dy() ) );
			//else 
				path.current()->transform( m_matrix );
		}

		if( !success() )
			setSuccess();
		path.next();
	}
}

