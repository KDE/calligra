/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONIZE_H__
#define __VPOLYGONIZE_H__

#include "vpath.h"
#include "vvisitor.h"

class VPolygonize : public VVisitor
{
public:
	VPolygonize( double flatness = 1.0 ) { m_flatness = flatness; }

	double flatness() const { return m_flatness; }
	void setFlatness( double flatness );

	virtual void visitVPath( VPath& path );
	virtual void visitVSegmentList( VSegmentList& segmentList );

private:
	double m_flatness;
};

#endif

