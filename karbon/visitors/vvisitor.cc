/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vdocument.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegmentlist.h"
#include "vvisitor.h"

void
VVisitor::visit( VObject& object )
{
	object.accept( *this );
}

void
VVisitor::visitVDocument( VDocument& document )
{
	VLayerListIterator itr( document.layers() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VVisitor::visitVGroup( VGroup& group )
{
	VObjectListIterator itr( group.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VVisitor::visitVLayer( VLayer& layer )
{
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VVisitor::visitVPath( VPath& path )
{
	VSegmentListListIterator itr( path.segmentLists() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
VVisitor::visitVSegmentList( VSegmentList& /*segmentList*/ )
{
}

void
VVisitor::visitVText( VText& /*text*/ )
{
}

