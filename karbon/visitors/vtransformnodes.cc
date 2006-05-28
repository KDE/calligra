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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "vpath.h"
#include "vsegment.h"
#include "vtransformnodes.h"
#include "vglobal.h"


VTransformNodes::VTransformNodes( const QMatrix& m )
	: m_matrix( m )
{
}

void
VTransformNodes::visitVSubpath( VSubpath& path )
{
	path.first();
	while( path.current() )
	{
		if( path.current()->isCurve() )
		{
			if( !path.current()->knotIsSelected() &&
				path.current()->pointIsSelected( 1 ) &&
				path.current()->next() &&
				path.current()->next()->isCurve() &&
				!path.current()->next()->pointIsSelected( 0 ) &&
				path.current()->isSmooth() )
			{
				// Do extra reverse trafo for smooth beziers
				QMatrix m2( m_matrix.m11(), m_matrix.m12(), m_matrix.m21(), m_matrix.m22(),
							-m_matrix.dx(), -m_matrix.dy() );
				path.current()->next()->setPoint( 0, VGlobal::transformPoint(path.current()->next()->point( 0 ), m2 ) );
			}
			if( path.current()->pointIsSelected( 0 ) &&
				path.current()->prev() &&
				path.current()->prev()->isCurve() &&
				!path.current()->prev()->knotIsSelected() &&
				!path.current()->prev()->pointIsSelected( 1 ) &&
				path.current()->prev()->isSmooth() )
			{
				// Do extra reverse trafo for smooth beziers
				QMatrix m2( m_matrix.m11(), m_matrix.m12(), m_matrix.m21(), m_matrix.m22(),
							-m_matrix.dx(), -m_matrix.dy() );
				path.current()->prev()->setPoint( 1, VGlobal::transformPoint(path.current()->prev()->point( 1 ), m2 ) );
			}
		}

		for( uint i = 0; i < path.current()->degree(); ++i )
		{
			if( path.current()->pointIsSelected( i ) )
				path.current()->setPoint( i, VGlobal::transformPoint(path.current()->point( i ), m_matrix ) );
		}

		if( !success() )
			setSuccess();
		path.next();
	}
}

