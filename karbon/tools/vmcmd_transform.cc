/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_transform.h"
#include "vpath.h"

VMCmdTransform::VMCmdTransform( KarbonPart* part, QPtrList<VObject> objects, const QWMatrix &mat )
	: VCommand( part, i18n("Transform Object") ), m_objects( objects ), m_mat( mat )
{
}

void
VMCmdTransform::execute()
{
	QPtrListIterator<VObject> itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat );
}

void
VMCmdTransform::unexecute()
{
	// inverting the matrix should undo the affine transformation
	QPtrListIterator<VObject> itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat.invert() );
}

