/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLPOLYGON_H__
#define __VCTOOLPOLYGON_H__

#include "vshapetool.h"

class KarbonPart;
class VPolygonDlg;

// A singleton state to create a polygon.

class VCToolPolygon : public VShapeTool
{
public:
	virtual ~VCToolPolygon();
	static VCToolPolygon* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const KoPoint& p, double d1, double d2 );

	virtual void showDialog() const;

protected:
	VCToolPolygon( KarbonPart* part );

private:
	static VCToolPolygon* s_instance;

	VPolygonDlg* m_dialog;
};

#endif

