/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VVISITOR_H__
#define __VVISITOR_H__

#include <qptrlist.h>


class VObject;
class VPath;
class VSegmentList;


class VVisitor
{
public:
	virtual void visit( VObject& object );
	virtual void visitVPath(
		VPath& /*path*/, QPtrList<VSegmentList>& /*lists*/ ) = 0;
};

#endif

