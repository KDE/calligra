/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include "vtool.h"

class KarbonPart;
class VCDlgRectangle;

// A singleton state to create a rectangle.

class VCToolRectangle : public VTool
{
public:
	virtual ~VCToolRectangle();
	static VCToolRectangle* instance( KarbonPart* part );

	virtual VCommand* createCmd( const QPoint& p, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolRectangle( KarbonPart* part );

private:
	static VCToolRectangle* s_instance;

	VCDlgRectangle* m_dialog;
};

#endif

