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

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include <qstring.h>

#include <koPoint.h>

#include "vtool.h"


class VComposite;


class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonView* view, const QString& name, bool polar = false );

	virtual QString name() { return m_name; }
	virtual QString icon() { return "14_polygon"; }
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
	virtual ~VShapeTool() {}

	virtual VComposite* shape( bool interactive = false ) const = 0;

	/**
	 * Output coordinates.
	 */
	KoPoint m_p;
	double m_d1;
	double m_d2;

private:
	void recalc();

	QString m_name;
	
	/// Calculate wiidth/height or radius/angle?
	bool m_isPolar;

	/// States:
	bool m_isSquare;
	bool m_isCentered;
};

#endif
