/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VHANDLETOOL_H__
#define __VHANDLETOOL_H__

#include <koRect.h>

#include "vtool.h"

class KarbonPart;
class QPainter;

// A singleton state to represent a handle.

enum { NODE_LT, NODE_MT, NODE_RT, NODE_LM, NODE_MM, NODE_RM, NODE_LB, NODE_MB, NODE_RB };

class VHandleTool : public VTool
{
public:
	virtual ~VHandleTool();
	static VHandleTool* instance( KarbonPart* part );

	void draw( QPainter& painter, const double zoomFactor );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	short activeNode() const;

	void drawBox( QPainter& painter, short index );

protected:
	VHandleTool( KarbonPart* part );

private:
	static VHandleTool* s_instance;

	KoRect m_boundingBox;
	KoRect m_nodes[9];
	short m_activeNode;
};

#endif

