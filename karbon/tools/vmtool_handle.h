/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLHANDLE_H__
#define __VMTOOLHANDLE_H__

#include "vtool.h"

class QPainter;
class KarbonPart;
class QRect;

// A singleton state to represent a handle.

enum { NODE_LT, NODE_MT, NODE_RT, NODE_LM, NODE_MM, NODE_RM, NODE_LB, NODE_MB, NODE_RB };

class VMToolHandle : public VTool
{
public:
	virtual ~VMToolHandle();
	static VMToolHandle* instance( KarbonPart* part );

	void draw( QPainter& painter, const double zoomFactor );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	short activeNode() const;

	void drawBox( QPainter& painter, short index );

protected:
	VMToolHandle( KarbonPart* part );

private:
	static VMToolHandle* s_instance;

	QRect m_bbox;
	QRect m_nodes[9];
	short m_activeNode;
};

#endif
