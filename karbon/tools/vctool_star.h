/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCTOOLSTAR_H__
#define __VCTOOLSTAR_H__

#include "vshapetool.h"

class KarbonPart;
class VStarDlg;

// A singleton state to create a star

class VCToolStar : public VShapeTool
{
public:
	virtual ~VCToolStar();
	static VCToolStar* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

    virtual void showDialog() const;

protected:
	VCToolStar( KarbonPart* part );

private:
	static VCToolStar* s_instance;

	VStarDlg* m_dialog;
};

#endif

