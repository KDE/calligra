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

#include <kdebug.h>

#include "karbon_view.h"
#include "vselection.h"
#include "vtoolfactory.h"
#include "vselecttool.h"
#include "vselectnodestool.h"
#include "vrotatetool.h"
#include "vsheartool.h"

VToolFactory::VToolFactory( KarbonView *view ) 
{
	m_selectTool		= new VSelectTool( view, "" );
	m_selectNodesTool	= new VSelectNodesTool( view, "" );
	m_rotateTool		= new VRotateTool( view, "" );
	m_shearTool			= new VShearTool( view, "" );
}

VToolFactory::~VToolFactory()
{
	delete m_selectTool;
	delete m_selectNodesTool;
	delete m_rotateTool;
	delete m_shearTool;
}

