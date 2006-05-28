/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#ifndef __VSELECTNODESTOOL_H__
#define __VSELECTNODESTOOL_H__

#include "vtool.h"

class QPointF;
class QRectF;

class VSelectNodesTool : public VTool
{
public:
	VSelectNodesTool( KarbonView *view );
	virtual ~VSelectNodesTool();

	virtual void activate();

	virtual void setup(KActionCollection *collection);
	virtual QString uiname() { return i18n( "Select Nodes Tool" ); }
	virtual enumToolType toolType() { return TOOL_SELECT; }
	virtual QString statusText();
	virtual uint priority() { return 1; }

protected:
	virtual void draw();

	virtual void setCursor() const;

	virtual void mouseButtonPress();
	virtual void mouseButtonRelease();
	virtual void mouseDragRelease();
	virtual void mouseDrag();
	virtual void rightMouseButtonPress();
	virtual void rightMouseButtonRelease();

	virtual bool keyReleased( Qt::Key );

	virtual void cancel();

private:
	enum { normal, dragging, moving, movingbezier1, movingbezier2 } m_state;

	void recalc();

	QRectF calcSelRect( const QPointF &pos ) const;

	// A list of temporary objects:
	VObjectList m_objects;

	QPointF m_current;

	bool m_select;
};

#endif

