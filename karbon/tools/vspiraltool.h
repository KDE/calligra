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

	virtual VPath* shape() const;

	virtual void showDialog() const;

private:
	VSpiralDlg* m_dialog;
};

#endif

