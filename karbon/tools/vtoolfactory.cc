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

#include <kdebug.h>

#include "karbon_view.h"
#include "vselection.h"
#include "vtoolfactory.h"
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

VToolFactory::VToolFactory( KarbonView *view ) 
{
	m_selectTool		= new VSelectTool( view, "" );
	m_selectNodesTool	= new VSelectNodesTool( view, "" );
	m_rotateTool		= new VRotateTool( view, "" );
	m_shearTool			= new VShearTool( view, "" );
	m_ellipseTool		= new VEllipseTool( view );
	m_gradientTool		= new VGradientTool( view, "" );
	m_patternTool		= new VPatternTool( view, "" );
	m_polygonTool		= new VPolygonTool( view );
	m_polylineTool		= new VPolylineTool( view, "" );
	m_rectangleTool		= new VRectangleTool( view );
	m_roundRectTool		= new VRoundRectTool( view );
	m_sinusTool			= new VSinusTool( view );
	m_spiralTool		= new VSpiralTool( view );
	m_starTool			= new VStarTool( view );
	m_textTool			= new VTextTool( view, "" );
}

VToolFactory::~VToolFactory()
{
	delete m_selectTool;
	delete m_selectNodesTool;
	delete m_rotateTool;
	delete m_shearTool;
	delete m_ellipseTool;
	delete m_gradientTool;
	delete m_patternTool;
	delete m_polygonTool;
	delete m_polylineTool;
	delete m_rectangleTool;
	delete m_roundRectTool;
	delete m_sinusTool;
	delete m_spiralTool;
	delete m_starTool;
	delete m_textTool;
}

