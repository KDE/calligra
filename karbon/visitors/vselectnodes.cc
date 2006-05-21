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


#include "vpath.h"
#include "vsegment.h"
#include "vselectnodes.h"
#include "vlayer.h"
#include "vdocument.h"

void
VSelectNodes::visitVSubpath( VSubpath& path )
{
	path.first();

	VSegment *curr = path.current();

	while( curr )
	{
		if( m_rect.isEmpty() )
		{
			for( int i = 0; i < curr->degree(); i++ )
				curr->selectPoint( i, m_select );

			setSuccess();
		}
		else
		{
			if( m_exclusive )
			{
				// we are in exclusive mode, so deselect all nodes first
				for( int i = 0; i < curr->degree(); i++ )
					curr->selectPoint( i, false );
			}

			if( curr->isCurve() )
			{
				// select all control points inside the selection rect
				for( int i = 0; i < curr->degree()-1; ++i )
				{
					if( m_rect.contains( curr->point( i ) ) )
					{
						curr->selectPoint( i, m_select );
						setSuccess();
					}
				}
				// if previous segment's knot is selected, select the current segment's first node as well
				VSegment* prev = curr->prev();
				if( prev && prev->isCurve() && prev->knotIsSelected() )
					curr->selectPoint( 0 );
			}	

			if( m_rect.contains( curr->knot() ) )
			{
				curr->selectKnot( m_select );
				// select the last control point before the knot. if segment is curve
				if( curr->isCurve() )
					curr->selectPoint( curr->degree()-2, m_select );

				// select first bezier point of next segment if:
				// - next segment is a curve and
				// - next segment is smooth or the current segment is the first one
				VSegment* next = curr->next();
				if( next && next->isCurve() && ( curr->isSmooth() || curr == path.getFirst() ) )
					next->selectPoint( 0, m_select );

				setSuccess();
			}
		}
		curr = curr->next();
	}
	// select first node as well
	if( path.isClosed() && path.getLast()->knotIsSelected() )
		path.getFirst()->selectKnot();
}

void
VSelectNodes::visitVLayer( VLayer& layer )
{
	VDocument* doc = (VDocument*)layer.parent();
	if ( ( layer.state() != VObject::deleted ) &&
	     ( ( doc->selectionMode() == VDocument::AllLayers ) ||
	       ( doc->selectionMode() == VDocument::VisibleLayers && ( layer.state() == VObject::normal || layer.state() == VObject::normal_locked ) ) ||
	       ( doc->selectionMode() == VDocument::SelectedLayers && layer.selected() ) ||
	       ( doc->selectionMode() == VDocument::ActiveLayer && doc->activeLayer() == &layer ) ) )
	{
		VObjectListIterator itr( layer.objects() );
		for( ; itr.current(); ++itr )
			itr.current()->accept( *this );
	}
}

void
VTestNodes::visitVSubpath( VSubpath& path )
{
	path.first();

	while( path.current() )
	{
		for( int i = 0; i < path.current()->degree(); i++ )
			if( m_rect.contains( path.current()->point( i ) ) ) //&&
				//path.current()->pointIsSelected( i ) )
				{
					m_segments.append( path.current() );
					setSuccess();
				}

		path.next();
	}
}

void
VTestNodes::visitVLayer( VLayer& layer )
{
	VDocument* doc = (VDocument*)layer.parent();
	if ( ( layer.state() != VObject::deleted ) &&
	     ( ( doc->selectionMode() == VDocument::AllLayers ) ||
	       ( doc->selectionMode() == VDocument::VisibleLayers && ( layer.state() == VObject::normal || layer.state() == VObject::normal_locked ) ) ||
	       ( doc->selectionMode() == VDocument::SelectedLayers && layer.selected() ) ||
	       ( doc->selectionMode() == VDocument::ActiveLayer && doc->activeLayer() == &layer ) ) )
	{
		VObjectListIterator itr( layer.objects() );
		for( ; itr.current(); ++itr )
			itr.current()->accept( *this );
	}
}

