/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPOLYGONTOOL_H__
#define __VPOLYGONTOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VPolygonDlg;

// A singleton state to create a polygon.

class VPolygonTool : public VShapeTool
{
public:
	virtual ~VPolygonTool();
	static VPolygonTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VPolygonTool( KarbonPart* part );

private:
	static VPolygonTool* s_instance;

	VPolygonDlg* m_dialog;
};

#endif

