/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPOLYGONTOOL_H__
#define __VPOLYGONTOOL_H__

#include "vshapetool.h"


class VPolygonDlg;


class VPolygonTool : public VShapeTool
{
public:
	VPolygonTool( KarbonView* view );
	virtual ~VPolygonTool();
	
	virtual void activate();

	virtual VPath* shape( bool decide = false ) const;

	virtual void showDialog() const;

private:
	VPolygonDlg* m_dialog;
};

#endif

