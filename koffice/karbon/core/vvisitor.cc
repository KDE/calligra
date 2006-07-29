/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "vcomposite.h"
#include "vdocument.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vselection.h"
#include "vvisitor.h"
#include "vimage.h"

bool
VVisitor::visit( VObject& object )
{
	m_success = false;

	object.accept( *this );

	return m_success;
}

void
VVisitor::visitVDocument( VDocument& document )
{
	VLayerListIterator itr( document.layers() );

	for( ; itr.current(); ++itr )
	{
		itr.current()->accept( *this );
	}
}

void
VVisitor::visitVGroup( VGroup& group )
{
	VObjectListIterator itr( group.objects() );

	for( ; itr.current(); ++itr )
	{
		itr.current()->accept( *this );
	}
}

void
VVisitor::visitVLayer( VLayer& layer )
{
	VObjectListIterator itr( layer.objects() );

	for( ; itr.current(); ++itr )
	{
		itr.current()->accept( *this );
	}
}

void
VVisitor::visitVPath( VPath& composite )
{
	VSubpathListIterator itr( composite.paths() );

	for( ; itr.current(); ++itr )
	{
		if( !itr.current()->isEmpty() )
			itr.current()->accept( *this );
	}
}

void
VVisitor::visitVSubpath( VSubpath& /*path*/ )
{
}

void
VVisitor::visitVSelection( VSelection& selection )
{
	VObjectListIterator itr( selection.objects() );

	for( ; itr.current() ; ++itr )
	{
		itr.current()->accept( *this );
	}
}

void
VVisitor::visitVText( VText& /*text*/ )
{
}

void
VVisitor::visitVImage( VImage& /*img*/ )
{
}

void 
VVisitor::visitVObject( VObject& /*object*/ )
{
}
