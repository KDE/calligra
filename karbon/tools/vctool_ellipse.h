/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolEllipse( KarbonPart* part );

private:
	static VCToolEllipse* s_instance;

	VCDlgEllipse* m_dialog;
};

#endif

