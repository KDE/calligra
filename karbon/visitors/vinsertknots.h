/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VINSERTKNOTS_H__
#define __VINSERTKNTOS_H__

#include "vpath.h"
#include "vvisitor.h"

class VInsertKnots : public VVisitor
{
public:
	VInsertKnots() { m_knots = 1; }

	uint knots() const { return m_knots; }
	void setKnots( uint knots );

	virtual void visitVPath(
		VPath& path, QPtrList<VSegmentList>& lists ) const;

private:
	uint m_knots;
};

#endif
