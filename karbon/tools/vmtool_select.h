/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vmtool.h"

class KarbonPart;

// A singleton state to select object(s)

class VMToolSelect : public VMTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

	virtual void startDragging();

protected:
	VMToolSelect( KarbonPart* part );

private:
	static VMToolSelect* s_instance;
	enum { NoTransform, Moving, Scaling } m_TransformState;
};

#endif
