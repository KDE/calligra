/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDRECTTOOL_H__
#define __VROUNDRECTTOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VRoundRectDlg;

// A singleton state to create a rectangle.

class VRoundRectTool : public VShapeTool
{
public:
	virtual ~VRoundRectTool();
	static VRoundRectTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VRoundRectTool( KarbonPart* part );

private:
	static VRoundRectTool* s_instance;

	VRoundRectDlg* m_dialog;
};

#endif

