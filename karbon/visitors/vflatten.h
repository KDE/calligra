/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFLATTEN_H__
#define __VFLATTE_H__


#include "vpath.h"
#include "vvisitor.h"


class VFlatten : public VVisitor
{
public:
	VFlatten( double flatness = 1.0 ) { m_flatness = flatness; }

	double flatness() const { return m_flatness; }
	void setFlatness( double flatness );

	virtual void visitVSegmentList( VSegmentList& segmentList );

private:
	double m_flatness;
};

#endif

