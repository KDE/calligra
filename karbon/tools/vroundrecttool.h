/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDRECTTOOL_H__
#define __VROUNDRECTTOOL_H__

#include "vshapetool.h"


class VRoundRectDlg;


class VRoundRectTool : public VShapeTool
{
public:
	VRoundRectTool( KarbonView* view );
	virtual ~VRoundRectTool();

	virtual void activate();

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

private:
	VRoundRectDlg* m_dialog;
};

#endif

