/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTARTOOL_H__
#define __VSTARTOOL_H__

#include "vshapetool.h"

class KarbonPart;
class VStarDlg;

// A singleton state to create a star

class VStarTool : public VShapeTool
{
public:
	virtual ~VStarTool();
	static VStarTool* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

    virtual void showDialog() const;

protected:
	VStarTool( KarbonPart* part );

private:
	static VStarTool* s_instance;

	VStarDlg* m_dialog;
};

#endif

