/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to select object(s)

class VMToolSelect : public VTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VMToolSelect( KarbonPart* part );

private:
	static VMToolSelect* s_instance;
	KarbonView* m_view;	// to get zoomFactor
	enum { NoTransform, Moving, Scaling, Rotating } m_TransformState;
};

#endif
