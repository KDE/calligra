/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vpolygonize.h"
#include "vpolygonizecmd.h"
#include "vselection.h"


VPolygonizeCmd::VPolygonizeCmd( VDocument *doc, double flatness )
		: VCommand( doc, i18n( "Polygonize" ) )
{
	m_selection = m_doc->selection()->clone();

	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

VPolygonizeCmd::~VPolygonizeCmd()
{
	delete( m_selection );
}

void
VPolygonizeCmd::execute()
{
	VPolygonize op( m_flatness );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
		op.visit( *itr.current() );
}

void
VPolygonizeCmd::unexecute()
{
}

