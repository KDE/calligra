/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VELLIPSETOOL_H__
#define __VELLIPSETOOL_H__

#include "vshapetool.h"


class VEllipseDlg;


class VEllipseTool : public VShapeTool
{
public:
	VEllipseTool( KarbonView* view );
	virtual ~VEllipseTool();

	virtual void activate();

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 );

private:
	VEllipseDlg* m_dialog;
};

#endif

