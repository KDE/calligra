/* This file is doc of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <klocale.h>

#include "vselection.h"
#include "vtransformcmd.h"


VTransformCmd::VTransformCmd( VDocument *doc, const QWMatrix& mat )
	: VCommand( doc, i18n( "Transform Objects" ) ), m_mat( mat )
{
	m_selection = document()->selection()->clone();

	if( m_selection->objects().count() == 1 )
		setName( i18n( "Transform Object" ) );
}

VTransformCmd::VTransformCmd( VDocument *doc, const QString& name )
		: VCommand( doc, name )
{
	m_selection = document()->selection()
		? new VSelection( *document()->selection() )
		: new VSelection();
}

VTransformCmd::~VTransformCmd()
{
	delete( m_selection );
}

void
VTransformCmd::execute()
{
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->transform( m_mat );
	}
}

void
VTransformCmd::unexecute()
{
	// inverting the matrix should undo the affine transformation
	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->transform( m_mat.invert() );
	}
}


VTranslateCmd::VTranslateCmd( VDocument *doc, double d1, double d2 )
		: VTransformCmd( doc, i18n( "Translate Objects" ) )
{
	if( m_selection->objects().count() == 1 )
		setName( i18n( "Translate Object" ) );

	m_mat.translate( d1, d2 );
}


VScaleCmd::VScaleCmd( VDocument *doc, const KoPoint& p, double s1, double s2 )
		: VTransformCmd( doc, i18n( "Scale Objects" ) )
{
	if( m_selection->objects().count() == 1 )
		setName( i18n( "Scale Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.scale( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}


VShearCmd::VShearCmd( VDocument *doc, const KoPoint& p, double s1, double s2 )
		: VTransformCmd( doc, i18n( "Shear Objects" ) )
{
	if( m_selection->objects().count() == 1 )
		setName( i18n( "Shear Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.shear( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}

VRotateCmd::VRotateCmd( VDocument *doc, const KoPoint& p, double angle )
		: VTransformCmd( doc, i18n( "Rotate Objects" ) )
{
	if( m_selection->objects().count() == 1 )
		setName( i18n( "Rotate Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.rotate( angle );
	m_mat.translate( -p.x(), -p.y() );
}

