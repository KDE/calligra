/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLELLIPSE_H__
#define __VCTOOLELLIPSE_H__

#include "vtool.h"

class KarbonPart;
class VCDlgEllipse;

// A singleton state to create an ellipse

class VCToolEllipse : public VTool
{
public:
	virtual ~VCToolEllipse();
	static VCToolEllipse* instance( KarbonPart* part );

	virtual VCommand* createCmdFromDialog( const QPoint& point );
	virtual VCommand* createCmdFromDragging( const QPoint& tl, const QPoint& br );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& tl, const QPoint& br );

protected:
	VCToolEllipse( KarbonPart* part );

private:
	static VCToolEllipse* s_instance;

	VCDlgEllipse* m_dialog;
};

#endif

