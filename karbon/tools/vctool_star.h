/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCTOOLSTAR_H__
#define __VCTOOLSTAR_H__

#include "vtool.h"

class KarbonPart;
class VCDlgStar;

// A singleton state to create a star

class VCToolStar : public VTool
{
public:
	virtual ~VCToolStar();
	static VCToolStar* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolStar( KarbonPart* part );

private:
	static VCToolStar* s_instance;

	VCDlgStar* m_dialog;
};

#endif

