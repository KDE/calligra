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

#ifndef __VSHEARTOOL_H__
#define __VSHEARTOOL_H__

#include "vtool.h"

class QPointF;

class VShearTool : public VTool
{
public:
	VShearTool( KarbonView *view );
	virtual ~VShearTool();

	virtual void activate();

	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Shear Tool" ); }
	virtual enumToolType toolType() { return TOOL_MANIPULATION; }
	virtual uint priority() { return 1; }
	virtual QString statusText();

protected:
	virtual void draw();

	virtual void setCursor() const;
	virtual void mouseButtonPress();
	virtual void mouseDrag();
	virtual void mouseDragRelease();

	virtual void cancel();

private:
	void recalc();

	QPointF m_center;
	double m_s1, m_s2;

	VHandleNode m_activeNode;

	// A list of temporary objects:
	VObjectList m_objects;
};

#endif

