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
#include "vpenciltool.h"
#include "vpolygontool.h"
#include "vpolylinetool.h"
#include "vrectangletool.h"
#include "vroundrecttool.h"
#include "vsinustool.h"
#include "vspiraltool.h"
#include "vstartool.h"
#include "vtexttool.h"

VToolController::VToolController( KarbonPart *part ) : m_part( part )
{
	//m_tools.setAutoDelete( true );
	m_activeTool = 0L;
}

void
VToolController::init()
{
	m_selectTool		= new VSelectTool( m_part, "" );
	m_selectNodesTool	= new VSelectNodesTool( m_part, "" );
	m_rotateTool		= new VRotateTool( m_part, "" );
	m_shearTool			= new VShearTool( m_part, "" );
	m_ellipseTool		= new VEllipseTool( m_part );
	m_gradientTool		= new VGradientTool( m_part, "" );
	m_patternTool		= new VPatternTool( m_part, "" );
	m_pencilTool		= new VPencilTool( m_part, "" );
	m_polygonTool		= new VPolygonTool( m_part );
	m_polylineTool		= new VPolylineTool( m_part, "" );
	m_rectangleTool		= new VRectangleTool( m_part );
	m_roundRectTool		= new VRoundRectTool( m_part );
	m_sinusTool			= new VSinusTool( m_part );
	m_spiralTool		= new VSpiralTool( m_part );
	m_starTool			= new VStarTool( m_part );
	m_textTool			= new VTextTool( m_part, "" );
	m_activeTool		= m_selectTool;
}

VToolController::~VToolController()
{
	delete m_selectTool;
	delete m_selectNodesTool;
	delete m_rotateTool;
	delete m_shearTool;
	delete m_ellipseTool;
	delete m_gradientTool;
	delete m_patternTool;
	delete m_pencilTool;
	delete m_polygonTool;
	delete m_polylineTool;
	delete m_rectangleTool;
	delete m_roundRectTool;
	delete m_sinusTool;
	delete m_spiralTool;
	delete m_starTool;
	delete m_textTool;
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
	return m_activeTool->mouseEvent( event, p );
}

bool
VToolController::keyEvent( QEvent* event )
{
	return m_activeTool->keyEvent( event );
}

