/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vpolygonize.h"
#include "vpolygonizecmd.h"


VPolygonizeCmd::VPolygonizeCmd( VDocument *doc, double flatness )
		: VCommand( doc, i18n( "Polygonize" ) )
{
	m_flatness = flatness > 0.0 ? flatness : 1.0;
	m_objects = doc->selection();
}

void
VPolygonizeCmd::execute()
{
	VPolygonize op( m_flatness );

	VObjectListIterator itr( m_objects.objects() );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VPolygonizeCmd::unexecute()
{
}

