/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VVISITOR_H__
#define __VVISITOR_H__


class VDocument;
class VGroup;
class VLayer;
class VObject;
class VPath;
class VSegmentList;
class VText;


class VVisitor
{
public:
	virtual void visit( VObject& object );

	virtual void visitVDocument( VDocument& /*document*/ ) {}
	virtual void visitVGroup( VGroup& /*group*/ ) {}
	virtual void visitVLayer( VLayer& /*layer*/ ) {}
	virtual void visitVPath( VPath& /*path*/ ) {}
	virtual void visitVSegmentList( VSegmentList& /*segmentList*/ ) {}
	virtual void visitVText( VText& /*text*/ ) {}

protected:
	/**
	 * Make this class "abstract".
	 */
	virtual ~VVisitor() {}
};

#endif

