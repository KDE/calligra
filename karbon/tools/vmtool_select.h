/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vtool.h"

class KarbonPart;

// A singleton state to select object(s)

class VMToolSelect : public VTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual VCommand* createCmd( const QPoint& p, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

	// selection-tool needs it's own event-filter:
	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VMToolSelect( KarbonPart* part );

private:
	static VMToolSelect* s_instance;
	enum { NoTransform, Moving, Scaling } m_TransformState;
};

#endif
