/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VVISITOR_H__
#define __VVISITOR_H__

class VPath;

class VVisitor
{
public:
	virtual void visitVPath(
		VPath& /*path*/, QPtrList<VSegmentList>& /*lists*/ ) const {}
};

#endif
