/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VELLIPSETOOL_H__
#define __VELLIPSETOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VEllipseDlg;

// A singleton state to create an ellipse

class VEllipseTool : public VShapeTool
{
public:
	virtual ~VEllipseTool();
	static VEllipseTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

protected:
	VEllipseTool( KarbonPart* part );

private:
	static VEllipseTool* s_instance;

	VEllipseDlg* m_dialog;
};

#endif

