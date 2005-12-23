/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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

#include <klocale.h>

#include "vfill.h"
#include "vfillcmd.h"
#include "vselection.h"
#include "vdocument.h"
#include "vcomposite.h"
#include "vgroup.h"
#include "vtext.h"

VFillCmd::VFillCmd( VDocument *doc, const VFill &fill, const QString &icon )
	: VCommand( doc, i18n( "Fill Objects" ), icon ), m_fill( fill )
{
	m_selection = document()->selection()->clone();

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Fill Object" ) );
}

VFillCmd::~VFillCmd()
{
	m_objects.clear();
	delete m_selection;
	m_selection = 0L;
}

void
VFillCmd::changeFill( const VFill &fill )
{
	m_fill = fill;

	if( !m_selection )
		m_selection = document()->selection()->clone();

	VObjectListIterator itr( m_selection->objects() );

	for( ; itr.current() ; ++itr )
	{
		visit( *itr.current() );
	}

	setSuccess( true );
}

void
VFillCmd::visitVGroup( VGroup& group )
{
	VObjectListIterator itr( group.objects() );

	for( ; itr.current() ; ++itr )
	{
		m_oldfills.push_back( VFill( *( itr.current()->fill() ) ) );
		itr.current()->setFill( m_fill );
		m_objects.append(itr.current() );
	}
}

void 
VFillCmd::visitVPath( VPath& composite )
{
	m_oldfills.push_back( VFill( *( composite.fill() ) ) );
	composite.setFill( m_fill );
	m_objects.append( &composite );
}

void
VFillCmd::visitVText( VText& text )
{
	m_oldfills.push_back( VFill( *( text.fill() ) ) );
	text.setFill( m_fill );
	m_objects.append( &text );
}

void
VFillCmd::execute()
{
	if( !m_selection )
		m_selection = document()->selection()->clone();
	VObjectListIterator itr( m_selection->objects() );

	for( ; itr.current() ; ++itr )
	{
		visit( *itr.current() );
	}

	setSuccess( true );
}

void
VFillCmd::unexecute()
{
	VObjectListIterator itr( m_objects );

	int i = 0;

	for( ; itr.current() ; ++itr )
	{
		itr.current()->setFill( m_oldfills[ i++ ] );
	}

	setSuccess( false );
}

