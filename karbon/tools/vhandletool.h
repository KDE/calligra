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

enum VHandleNode
{
	node_lt,
	node_mt,
	node_rt,
	node_lm,
	node_mm,
	node_rm,
	node_lb,
	node_mb,
	node_rb
};


class VHandleTool : public VTool
{
public:
	virtual ~VHandleTool();
	static VHandleTool* instance( KarbonPart* part );

	void draw( QPainter& painter, const double zoomFactor );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	VHandleNode activeNode() const { return m_activeNode; }

protected:
	VHandleTool( KarbonPart* part );

private:
	static VHandleTool* s_instance;

	KoRect m_boundingBox;
	KoRect m_nodes[9];
	VHandleNode m_activeNode;
};

#endif

