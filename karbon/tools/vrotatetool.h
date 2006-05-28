/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vselection.h"
#include "vtool.h"

class QPointF;

class VRotateTool : public VTool
{
public:
	VRotateTool( KarbonView *view );
	virtual ~VRotateTool();

	virtual void activate();

	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Rotate Tool" ); }
	virtual enumToolType toolType() { return TOOL_MANIPULATION; }
	virtual QString statusText();
	virtual uint priority() { return 0; }

protected:
	virtual void draw();

	virtual void mouseButtonPress();
	virtual void mouseDrag();
	virtual void mouseDragRelease();

	virtual void cancel();

private:
	void recalc();

	QPointF m_center;
	double m_angle;

	VHandleNode m_activeNode;

	// A list of temporary objects:
	VObjectList m_objects;
};

#endif

