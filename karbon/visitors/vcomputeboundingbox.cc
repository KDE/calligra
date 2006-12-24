/* This file is part of the KDE project
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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


#include "vcomputeboundingbox.h"
#include "vdocument.h"
#include "vlayer.h"
#include "vgroup.h"
#include "vcomposite.h"
#include "vtext.h"
#include "vimage.h"

VComputeBoundingBox::VComputeBoundingBox( bool omitHidden )
: m_omitHidden( omitHidden )
{
}

void
VComputeBoundingBox::visitVDocument( VDocument& document )
{
	VLayerListIterator itr( document.layers() );

	for( ; itr.current(); ++itr )
	{
		// do not use hidden layers
		if( m_omitHidden && ! isVisible( itr.current() ) )
			continue;
		itr.current()->accept( *this );
	}
}

void
VComputeBoundingBox::visitVLayer( VLayer& layer )
{
	VObjectListIterator itr( layer.objects() );

	for( ; itr.current(); ++itr )
	{
		// do not export hidden objects
		if( m_omitHidden && ! isVisible( itr.current() ) )
			continue;
		itr.current()->accept( *this );
	}
}

void
VComputeBoundingBox::visitVGroup( VGroup& group )
{
	VObjectListIterator itr( group.objects() );

	for( ; itr.current(); ++itr )
	{
		// do not use hidden child objects
		if( m_omitHidden && ! isVisible( itr.current() ) )
			continue;
		itr.current()->accept( *this );
	}
}

void
VComputeBoundingBox::visitVPath( VPath& composite )
{
	m_boundingBox |= composite.boundingBox();
}

void
VComputeBoundingBox::visitVText( VText& text )
{
	m_boundingBox |= text.boundingBox();
}

void
VComputeBoundingBox::visitVImage( VImage& img )
{
	m_boundingBox |= img.boundingBox();
}

bool
VComputeBoundingBox::isVisible( const VObject* object ) const
{
	return object->state() != VObject::hidden && object->state() != VObject::hidden_locked;
}

const KoRect&
VComputeBoundingBox::boundingRect() const
{
	return m_boundingBox;
}
