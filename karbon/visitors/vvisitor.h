/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VVISITOR_H__
#define __VVISITOR_H__

class VShape;
class VPath;
class VSegmentList;

class VVisitor
{
public:
	virtual void visit( VShape& object );
	virtual void visitVPath(
		VPath& /*path*/, QPtrList<VSegmentList>& /*lists*/ ) = 0;
};

#endif

