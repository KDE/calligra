/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include "vshapetool.h"

class KarbonPart;
class VRectangleDlg;

// A singleton state to create a rectangle.

class VCToolRectangle : public VShapeTool
{
public:
	virtual ~VCToolRectangle();
	static VCToolRectangle* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

protected:
	VCToolRectangle( KarbonPart* part );

private:
	static VCToolRectangle* s_instance;

	VRectangleDlg* m_dialog;
};

#endif

