/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLPOLYGON_H__
#define __VCTOOLPOLYGON_H__

#include "vtool.h"

class KarbonPart;
class VCDlgPolygon;

// A singleton state to create a polygon.

class VCToolPolygon : public VTool
{
public:
	virtual ~VCToolPolygon();
	static VCToolPolygon* instance( KarbonPart* part );

	virtual VCommand* createCmd( const QPoint& p, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VCToolPolygon( KarbonPart* part );

private:
	static VCToolPolygon* s_instance;

	VCDlgPolygon* m_dialog;
};

#endif

