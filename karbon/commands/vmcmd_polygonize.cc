/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_polygonize.h"
#include "vpath.h"
#include "vpolygonize.h"

VMCmdPolygonize::VMCmdPolygonize( KarbonPart* part, const VObjectList& objects,
	double flatness )
		: VCommand( part, i18n( "Polygonize" ) ), m_objects( objects )
{
	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

void
VMCmdPolygonize::execute()
{
	VPolygonize op( m_flatness );

	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VMCmdPolygonize::unexecute()
{
}

