/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLROUNDRECT_H__
#define __VCTOOLROUNDRECT_H__

#include "vtool.h"

class KarbonPart;
class VCDlgRoundRect;

// A singleton state to create a rectangle.

class VCToolRoundRect : public VTool
{
public:
	virtual ~VCToolRoundRect();
	static VCToolRoundRect* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolRoundRect( KarbonPart* part );

private:
	static VCToolRoundRect* s_instance;

	VCDlgRoundRect* m_dialog;
};

#endif

