/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTARTOOL_H__
#define __VSTARTOOL_H__

#include "vshapetool.h"


class VStarDlg;


class VStarTool : public VShapeTool
{
public:
	VStarTool( KarbonView* view );
	virtual ~VStarTool();

	virtual void activate();

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

private:
	VStarDlg* m_dialog;
};

#endif

