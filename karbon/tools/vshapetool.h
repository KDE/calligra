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

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include <qstring.h>

#include <KoPoint.h>

#include "vtool.h"


class VPath;
class VCursor;

class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonView *view, const char *name, bool polar = false );

	virtual enumToolType toolType() { return TOOL_SHAPE; }
	virtual QString statusText();
	virtual QString contextHelp();

	virtual void activate();

protected:
	virtual void draw();

	virtual void mouseButtonPress();
	virtual void mouseButtonRelease();
	virtual void mouseDrag();
	virtual void mouseDragRelease();
	virtual void mouseDragShiftPressed();
	virtual void mouseDragCtrlPressed();
	virtual void mouseDragShiftReleased();
	virtual void mouseDragCtrlReleased();

	virtual void cancel();

	// Make it "abstract":
	virtual ~VShapeTool();

	virtual VPath* shape( bool interactive = false ) const = 0;

	/**
	 * Output coordinates.
	 */
	KoPoint m_p;
	double m_d1;
	double m_d2;

private:
	void recalc();

	/// Calculate width/height or radius/angle?
	bool m_isPolar;

	/// States:
	bool m_isSquare;
	bool m_isCentered;

	QCursor* m_cursor;
};

#endif
