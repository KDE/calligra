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

#ifndef __VTOOLCONTROLLER_H__
#define __VTOOLCONTROLLER_H__

#include <qdict.h>
#include <KoPoint.h>

class QEvent;

class VTool;
class KarbonPart;
class KarbonView;

class VToolController
{
public:
	VToolController( KarbonPart *part );
	virtual ~VToolController();

	void init();

	void registerTool( VTool *tool );
	void unregisterTool( VTool *tool );

	void setActiveTool( VTool * );
	VTool *activeTool() const { return m_activeTool; }

	KarbonView *activeView() const { return m_activeView; }
	void setActiveView( KarbonView *view ) { m_activeView = view; }

	bool mouseEvent( QMouseEvent* event, const KoPoint& );
	bool keyEvent( QEvent* event );

	const QDict<VTool> &tools() { return m_tools; }

private:
	KarbonPart			*m_part;
	KarbonView			*m_activeView;
	VTool				*m_activeTool;
	QDict<VTool>		m_tools;
};

#endif

