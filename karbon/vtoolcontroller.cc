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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "karbon_tool_registry.h"
#include "karbon_part.h"
#include <vselection.h>
#include "vtoolcontroller.h"
#include "vtool.h"

VToolController::VToolController( KarbonPart *part ) : m_part( part )
{
	//m_tools.setAutoDelete( true );
	m_activeTool = 0L;
}

void
VToolController::init()
{
	KarbonToolRegistry::instance()->createTools( m_part->actionCollection(), m_part);
}

VToolController::~VToolController()
{
}

void
VToolController::setActiveTool( VTool *tool )
{
	if( m_activeTool )
		m_activeTool->deactivate();

	if( m_activeTool == tool )
		m_activeTool->showDialog();
	else
	{
		m_activeTool = tool;
		m_activeTool->activateAll();
	}
}

void
VToolController::registerTool( VTool *tool )
{
	if( !m_tools.find( tool->name() ) )
		m_tools.insert( tool->name(), tool );
	//kdDebug(38000) << "active tool : " << m_activeTool->name() << endl;
}

void
VToolController::unregisterTool( VTool *tool )
{
	// tool->name() is not valid in VTool destructor
	QDictIterator<VTool> it( m_tools );
	for( ; it.current(); ++it )
		if (it.current() == tool)
		{
			m_tools.remove(it.currentKey());
			return;
		}
}

bool
VToolController::mouseEvent( QMouseEvent* event, const KoPoint &p )
{
	if( !m_activeTool ) {
		return false;
	}

	return m_activeTool->mouseEvent( event, p );
}

bool
VToolController::keyEvent( QEvent* event )
{
	if( !m_activeTool ) {
		return false;
	}

	return m_activeTool->keyEvent( event );
}

