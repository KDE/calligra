/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VRECTANGLETOOL_H__
#define __VRECTANGLETOOL_H__

#include "vshapetool.h"


class VRectangleDlg;


class VRectangleTool : public VShapeTool
{
public:
	VRectangleTool( KarbonView* view );
	virtual ~VRectangleTool();

	virtual void activate();

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 );

private:
	VRectangleDlg* m_dialog;
};

#endif

