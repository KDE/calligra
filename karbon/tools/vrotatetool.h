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

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vselection.h"
#include "vtool.h"


class VRotateTool : public VTool
{
public:
	VRotateTool( KarbonView* view );
	virtual ~VRotateTool();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor( const KoPoint& current ) const;
	virtual void mouseButtonPress( const KoPoint& current );
	virtual void mouseDrag( const KoPoint& current );
	virtual void mouseDragRelease( const KoPoint& current );

private:
	void recalc();

	KoPoint m_center;
	double m_angle;

	VHandleNode m_activeNode;

	// A list of temporary objects:
	VObjectList m_objects;
};

#endif

