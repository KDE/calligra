/* This file is doc of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vtransformcmd.h"


VTransformCmd::VTransformCmd( VDocument *doc, const QWMatrix& mat )
		: VCommand( doc, i18n( "Transform Objects" ) ), m_mat( mat )
{
	if( m_objects.objects().count() == 1 )
		setName( i18n( "Transform Object" ) );

	m_objects = doc->selection();
}

VTransformCmd::VTransformCmd( VDocument *doc, const QString& name )
		: VCommand( doc, name )
{
	m_objects = doc->selection();
}

void
VTransformCmd::execute()
{
	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->transform( m_mat );
	}
}

void
VTransformCmd::unexecute()
{
	// inverting the matrix should undo the affine transformation
	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->transform( m_mat.invert() );
	}
}


VTranslateCmd::VTranslateCmd( VDocument *doc, double d1, double d2 )
		: VTransformCmd( doc, i18n( "Translate Objects" ) )
{
	if( m_objects.objects().count() == 1 )
		setName( i18n( "Translate Object" ) );

	m_mat.translate( d1, d2 );
}


VScaleCmd::VScaleCmd( VDocument *doc, const KoPoint& p, double s1, double s2 )
		: VTransformCmd( doc, i18n( "Scale Objects" ) )
{
	if( m_objects.objects().count() == 1 )
		setName( i18n( "Scale Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.scale( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}


VShearCmd::VShearCmd( VDocument *doc, const KoPoint& p, double s1, double s2 )
		: VTransformCmd( doc, i18n( "Shear Objects" ) )
{
	if( m_objects.objects().count() == 1 )
		setName( i18n( "Shear Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.shear( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}

VRotateCmd::VRotateCmd( VDocument *doc, const KoPoint& p, double angle )
		: VTransformCmd( doc, i18n( "Rotate Objects" ) )
{
	if( m_objects.objects().count() == 1 )
		setName( i18n( "Rotate Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.rotate( angle );
	m_mat.translate( -p.x(), -p.y() );
}

