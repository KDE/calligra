/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTTOOL_H__
#define __VSELECTTOOL_H__

#include "vtool.h"
#include "vselection.h"

class VSelectTool : public VTool
{
public:
	VSelectTool( KarbonView* view );
	virtual ~VSelectTool();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor( const KoPoint& current ) const;

	virtual void mouseButtonPress( const KoPoint& current );
	virtual void mouseDrag( const KoPoint& current );
	virtual void mouseDragRelease( const KoPoint& current );
	virtual void mouseDragCtrlPressed( const KoPoint& current );
	virtual void mouseDragCtrlReleased( const KoPoint& current );

private:
	enum { normal, moving, scaling, rotating } m_state;
	enum { none, lockx, locky } m_lock;

	double m_s1;
	double m_s2;
	KoPoint m_sp;

	VHandleNode m_activeNode;
};

#endif

