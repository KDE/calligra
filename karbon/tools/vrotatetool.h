/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vselection.h"
#include "vtool.h"


class VRotateTool : public VTool
{
public:
	VRotateTool( KarbonView* view );
	virtual ~VRotateTool();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor( const KoPoint& current ) const;
	virtual void mouseButtonPress( const KoPoint& current );
	virtual void mouseDrag( const KoPoint& current );
	virtual void mouseDragRelease( const KoPoint& current );

private:
	void recalc();

	KoPoint m_center;
	double m_angle;

	VHandleNode m_activeNode;
};

#endif

