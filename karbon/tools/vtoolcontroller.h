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

#ifndef __VTOOLCONTROLLER_H__
#define __VTOOLCONTROLLER_H__

#include <qdict.h>
#include <koPoint.h>

class QEvent;

class VTool;
class KarbonPart;
class KarbonView;
class VSelectTool;
class VSelectNodesTool;
class VRotateTool;
class VShearTool;
class VEllipseTool;
class VGradientTool;
class VPatternTool;
class VPencilTool;
class VPolygonTool;
class VPolylineTool;
class VRectangleTool;
class VRoundRectTool;
class VSinusTool;
class VSpiralTool;
class VStarTool;
class VTextTool;

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
	VSelectTool			*m_selectTool;
	VSelectNodesTool	*m_selectNodesTool;
	VRotateTool			*m_rotateTool;
	VShearTool			*m_shearTool;
	VEllipseTool		*m_ellipseTool;
	VGradientTool		*m_gradientTool;
	VPatternTool		*m_patternTool;
	VPencilTool			*m_pencilTool;
	VPolygonTool		*m_polygonTool;
	VPolylineTool		*m_polylineTool;
	VRectangleTool		*m_rectangleTool;
	VRoundRectTool		*m_roundRectTool;
	VSinusTool			*m_sinusTool;
	VSpiralTool			*m_spiralTool;
	VStarTool			*m_starTool;
	VTextTool			*m_textTool;
};

#endif

