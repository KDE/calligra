/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSPIRALTOOL_H__
#define __VSPIRALTOOL_H__

#include "vshapetool.h"


class VSpiralDlg;


class VSpiralTool : public VShapeTool
{
public:
	VSpiralTool( KarbonView* view );
	virtual ~VSpiralTool();

	virtual void activate();

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

private:
	VSpiralDlg* m_dialog;
};

#endif

