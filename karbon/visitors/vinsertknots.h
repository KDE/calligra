/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VINSERTKNOTS_H__
#define __VINSERTKNOTS_H__

#include "vpath.h"
#include "vvisitor.h"

class VInsertKnots : public VVisitor
{
public:
	VInsertKnots( uint knots = 1 ) { m_knots = knots; }

	uint knots() const { return m_knots; }
	void setKnots( uint knots );

	virtual void visitVPath(
		VPath& path, QPtrList<VSegmentList>& lists );

private:
	uint m_knots;
};

#endif

