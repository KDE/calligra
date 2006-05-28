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

#include <q3dict.h>
#include <QMouseEvent>
#include <QEvent>
#include <QPointF>

class QEvent;

class VTool;
class KarbonView;
class VToolBox;

class VToolController
{
public:
	VToolController( KarbonView *view );
	virtual ~VToolController();

	void init();

	void registerTool( VTool *tool );
	void unregisterTool( VTool *tool );

	void setCurrentTool( VTool * );
	VTool *currentTool() const { return m_currentTool; }

	bool mouseEvent( QMouseEvent* event, const QPointF& );
	bool keyEvent( QEvent* event );

	const Q3Dict<VTool> &tools() { return m_tools; }

	void setUp( KActionCollection *ac, VToolBox * toolbox );
	void resetToolBox( VToolBox * toolbox );
	VTool *findTool( const QString &toolName ) const;

	// Called when the toolbox is deleted because the view was made inactive in favour of another view
	void youAintGotNoToolBox();

private:
	KarbonView		*m_view;
	VTool			*m_currentTool;
	Q3Dict<VTool>		 m_tools;
	VToolBox 		*m_toolBox;
	bool			 m_setup;
};

#endif

