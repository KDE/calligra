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

	virtual VCommand* createCmdFromDialog( const QPoint& point );
	virtual VCommand* createCmdFromDragging( const QPoint& tl, const QPoint& br );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& tl, const QPoint& br );

protected:
	VCToolRoundRect( KarbonPart* part );

private:
	static VCToolRoundRect* s_instance;

	VCDlgRoundRect* m_dialog;
};

#endif

