/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vpolygonize.h"
#include "vpolygonizecmd.h"


VPolygonizeCmd::VPolygonizeCmd( KarbonPart* part, const VObjectList& objects,
	double flatness )
		: VCommand( part, i18n( "Polygonize" ) ), m_objects( objects )
{
	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

void
VPolygonizeCmd::execute()
{
	VPolygonize op( m_flatness );

	VObjectListIterator itr( m_objects );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VPolygonizeCmd::unexecute()
{
}

