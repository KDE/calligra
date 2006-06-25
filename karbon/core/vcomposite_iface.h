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

#ifndef __VCOMPOSITE_IFACE_H__
#define __VCOMPOSITE_IFACE_H__

#include "vobject_iface.h"

/*
TODO: porting to DBUS ?
class VPath;

class VPathIface : public VObjectIface
{
	K_DCOP

public:
	VPathIface( VPath *composite );

k_dcop:
	bool moveTo( double x, double y );
	bool lineTo( double x, double y );
	bool curveTo( double x1, double y1, double x2, double y2, double x3, double y3 );
	bool curve1To( double x2, double y2, double x3, double y3 );
	bool curve2To( double x1, double y1, double x2, double y2 );
	bool arcTo( double x1, double y1, double x2, double y2, double r );
	void close();

	//bool drawCenterNode() const;
	//void setDrawCenterNode( bool drawCenterNode = true );

private:
	VPath *m_composite;
};
*/
#endif
