/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCTOOLROUNDRECT_H__
#define __VCTOOLROUNDRECT_H__

#include "vshapetool.h"

class KarbonPart;
class VRoundRectDlg;

// A singleton state to create a rectangle.

class VCToolRoundRect : public VShapeTool
{
public:
	virtual ~VCToolRoundRect();
	static VCToolRoundRect* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VCToolRoundRect( KarbonPart* part );

private:
	static VCToolRoundRect* s_instance;

	VRoundRectDlg* m_dialog;
};

#endif

