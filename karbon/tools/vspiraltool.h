/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSPIRALTOOL_H__
#define __VSPIRALTOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VSpiralDlg;

// A singleton state to create a spiral.

class VSpiralTool : public VShapeTool
{
public:
	virtual ~VSpiralTool();
	static VSpiralTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VSpiralTool( KarbonPart* part );

private:
	static VSpiralTool* s_instance;

	VSpiralDlg* m_dialog;
};

#endif

