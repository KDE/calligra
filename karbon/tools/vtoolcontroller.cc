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

#include "karbon_part.h"
#include <vselection.h>
#include "vtoolcontroller.h"
#include "vselecttool.h"
#include "vselectnodestool.h"
#include "vrotatetool.h"
#include "vsheartool.h"
#include "vellipsetool.h"
#include "vgradienttool.h"
#include "vpatterntool.h"
#include "vpolygontool.h"
#include "vpolylinetool.h"
#include "vrectangletool.h"
#include "vroundrecttool.h"
#include "vsinustool.h"
#include "vspiraltool.h"
#include "vstartool.h"
#include "vtexttool.h"
#include "vcliparttool.h"

VToolController::VToolController( KarbonPart *part ) : m_part( part )
{
	//m_tools.setAutoDelete( true );
}

void
VToolController::init()
{
	new VSelectTool( m_part, "" );
	new VSelectNodesTool( m_part, "" );
	new VRotateTool( m_part, "" );
	new VShearTool( m_part, "" );
	new VEllipseTool( m_part );
	new VGradientTool( m_part, "" );
	new VPatternTool( m_part, "" );
	new VPolygonTool( m_part );
	new VPolylineTool( m_part, "" );
	new VRectangleTool( m_part );
	new VRoundRectTool( m_part );
	new VSinusTool( m_part );
	new VSpiralTool( m_part );
	new VStarTool( m_part );
	new VTextTool( m_part, "" );
	new VClipartTool( m_part, "" );
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
	m_activeTool = tool;
	kdDebug() << "active tool : " << m_activeTool->name() << endl;
}

bool
VToolController::mouseEvent( QMouseEvent* event, const KoPoint &p )
{
	//kdDebug()  << "Count : " << m_tools.count() << endl;
	kdDebug() << "CAlling : " << m_activeTool->name() << endl;
	return m_activeTool->mouseEvent( event, p );
}

bool
VToolController::keyEvent( QEvent* event )
{
	return m_activeTool->keyEvent( event );
}

