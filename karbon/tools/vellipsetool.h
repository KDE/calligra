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

	virtual VPath* shape( bool decide = false ) const;
    void refreshUnit();

private:
	VEllipseDlg* m_dialog;
};

#endif

