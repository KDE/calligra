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

#include "vcomposite_iface.h"
#include "vcomposite.h"

#include <QPointF>

VPathIface::VPathIface( VPath *composite )
	: VObjectIface( composite ), m_composite( composite )
{
}

bool
VPathIface::moveTo( double x, double y )
{
	return m_composite->moveTo( QPointF( x, y ) );
}

bool 
VPathIface::lineTo( double x, double y )
{
	return m_composite->lineTo( QPointF( x, y ) );
}

bool 
VPathIface::curveTo( double x1, double y1, double x2, double y2, double x3, double y3 )
{
	return m_composite->curveTo( QPointF( x1, y1 ), QPointF( x2, y2 ), QPointF( x3, y3 ) );
}

bool 
VPathIface::curve1To( double x2, double y2, double x3, double y3 )
{
	return m_composite->curve1To( QPointF( x2, y2 ), QPointF( x3, y3 ) );
}

bool 
VPathIface::curve2To( double x1, double y1, double x2, double y2 )
{
	return m_composite->curve2To( QPointF( x1, y1 ), QPointF( x2, y2 ) );
}

bool 
VPathIface::arcTo( double x1, double y1, double x2, double y2, double r )
{
	return m_composite->arcTo( QPointF( x1, y1 ), QPointF( x2, y2 ), r );
}

void 
VPathIface::close()
{
	m_composite->close();
}

