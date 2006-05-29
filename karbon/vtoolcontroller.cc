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

#include "karbon_tool_registry.h"
#include "karbon_part.h"
#include <vselection.h>
#include "vtoolcontroller.h"
#include "vtool.h"
#include "vtoolbox.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>

VToolController::VToolController( KarbonView *view ) : m_view( view ), m_currentTool( 0L ), m_setup( false )
{
    m_tools.setAutoDelete( true );
}

void
VToolController::init()
{
}

VToolController::~VToolController()
{
}

void
VToolController::setCurrentTool( VTool *tool )
{
	if( m_currentTool )
	{
		m_currentTool->action()->setChecked( false );
		m_currentTool->deactivate();
	}

	if( m_currentTool && m_currentTool == tool )
		m_currentTool->showDialog();
	else
	{
		m_currentTool = tool;

		if( ! tool )
			return;

		m_currentTool->action()->setChecked( true );
		m_currentTool->action()->trigger();
	}
	m_toolBox->slotSetTool( tool->objectName() );
}

void
VToolController::registerTool( VTool *tool )
{
	if( !m_tools.find( tool->objectName() ) )
		m_tools.insert( tool->objectName(), tool );
	//kDebug(38000) << "active tool : " << m_currentTool->name() << endl;
}

void
VToolController::unregisterTool( VTool *tool )
{
	// tool->name() is not valid in VTool destructor
	Q3DictIterator<VTool> it( m_tools );
	for( ; it.current(); ++it )
		if (it.current() == tool)
		{
			m_tools.remove(it.currentKey());
			return;
		}
}

bool
VToolController::mouseEvent( QMouseEvent* event, const QPointF &p )
{
	if( !m_currentTool ) {
		return false;
	}

	return m_currentTool->mouseEvent( event, p );
}

bool
VToolController::keyEvent( QEvent* event )
{
	if( !m_currentTool ) {
		return false;
	}

	return m_currentTool->keyEvent( event );
}

void
VToolController::setUp( KActionCollection *ac, VToolBox * toolbox )
{
	if( m_setup )
	{
		resetToolBox( toolbox);
		return;
	}

	KarbonToolRegistry::instance()->createTools( ac, m_view );

	m_toolBox = toolbox;

	Q3DictIterator<VTool> it( m_tools );
	for( ; it.current(); ++it )
		toolbox->registerTool( it.current() );

	toolbox->setupTools();

	VTool *t = findTool( "tool_select" );
	setCurrentTool(t);

	m_setup = true;
}

void
VToolController::resetToolBox( VToolBox * toolbox )
{
	m_toolBox = toolbox;

	Q3DictIterator<VTool> it( m_tools );
	for( ; it.current(); ++it )
		toolbox->registerTool( it.current() );

	toolbox->setupTools();

	if( m_currentTool )
	{
		// restore the old current tool
		setCurrentTool( m_currentTool );
		m_currentTool = 0;
	}
}

VTool *
VToolController::findTool( const QString &toolName ) const
{
	VTool *tool = 0;
	Q3DictIterator<VTool> it( m_tools );
	for( ; it.current(); ++it )
		if( it.current()->objectName() == toolName )
			return it.current();
	return tool;
}

void
VToolController::youAintGotNoToolBox()
{
	m_toolBox = 0;
	//m_currentTool = currentTool();
}

