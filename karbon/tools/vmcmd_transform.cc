/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_transform.h"
#include "vpath.h"

VMCmdTransform::VMCmdTransform( KarbonPart* part, const VObjectList& objects, const QWMatrix &mat )
	: VCommand( part, i18n( "Transform Objects" ) ), m_objects( objects ), m_mat( mat )
{
	if( m_objects.count() == 1 )
		setName( i18n( "Transform Object" ) );
}

VMCmdTransform::VMCmdTransform( KarbonPart* part, const VObjectList& objects, const QString &name )
	: VCommand( part, name ), m_objects( objects )
{
}

void
VMCmdTransform::execute()
{
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat );
}

void
VMCmdTransform::unexecute()
{
	// inverting the matrix should undo the affine transformation
	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat.invert() );
}

VMCmdTranslate::VMCmdTranslate( KarbonPart* part, const VObjectList& objects, double d1, double d2 )
	: VMCmdTransform( part, objects, i18n( "Translate Objects" ) )
{
	if( m_objects.count() == 1 )
		setName( i18n( "Translate Object" ) );

	m_mat.translate( d1, d2 );
}

VMCmdScale::VMCmdScale( KarbonPart* part, const VObjectList& objects, const QPoint &p, double s1, double s2 )
	: VMCmdTransform( part, objects, i18n( "Scale Objects" ) )
{
	if( m_objects.count() == 1 )
		setName( i18n( "Scale Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.scale( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}

VMCmdShear::VMCmdShear( KarbonPart* part, const VObjectList& objects, const QPoint &p, double s1, double s2 )
	: VMCmdTransform( part, objects, i18n( "Shear Objects" ) )
{
	if( m_objects.count() == 1 )
		setName( i18n( "Shear Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.shear( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}

VMCmdRotate::VMCmdRotate( KarbonPart* part, const VObjectList& objects, const QPoint &p, double angle )
	: VMCmdTransform( part, objects, i18n( "Rotate Objects" ) )
{
	if( m_objects.count() == 1 )
		setName( i18n( "Rotate Object" ) );

	m_mat.translate( p.x(), p.y() );
	m_mat.rotate( angle );
	m_mat.translate( -p.x(), -p.y() );
}

