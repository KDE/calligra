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

	virtual VPath* shape( bool interactive = false ) const;

	virtual void showDialog() const;
    void refreshUnit();
private:
	VStarDlg* m_dialog;
};

#endif

