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

#ifndef __VTOOLFACTORY_H__
#define __VTOOLFACTORY_H__

class VSelectTool;
class VSelectNodesTool;
class VRotateTool;
class VShearTool;

class VClipartTool;
class VEllipseTool;
class VGradientTool;
class VPatternTool;
class VPolygonTool;
class VPolylineTool;
class VRectangleTool;
class VRoundRectTool;
class VSinusTool;
class VSpiralTool;
class VStarTool;
class VTextTool;

class VToolFactory
{
public:
	VToolFactory( KarbonView *view );
	virtual ~VToolFactory();

private:
	VSelectTool			*m_selectTool;
	VSelectNodesTool	*m_selectNodesTool;
	VRotateTool			*m_rotateTool;
	VShearTool			*m_shearTool;
	VEllipseTool		*m_ellipseTool;
	VGradientTool		*m_gradientTool;
	VPatternTool		*m_patternTool;
	VPolygonTool		*m_polygonTool;
	VPolylineTool		*m_polylineTool;
	VRectangleTool		*m_rectangleTool;
	VRoundRectTool		*m_roundRectTool;
	VSinusTool			*m_sinusTool;
	VSpiralTool			*m_spiralTool;
	VStarTool			*m_starTool;
	VTextTool			*m_textTool;
	VClipartTool		*m_clipartTool;
};

#endif

