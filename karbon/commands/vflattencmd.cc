/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vflatten.h"
#include "vflattencmd.h"
#include "vgroup.h"
#include "vselection.h"


VFlattenCmd::VFlattenCmd( VDocument *doc, double flatness )
		: VCommand( doc, i18n( "Flatten" ) )
{
	m_selection = m_doc->selection()->clone();

	m_flatness = flatness > 0.0 ? flatness : 1.0;
}

VFlattenCmd::~VFlattenCmd()
{
	delete( m_selection );
}

void
VFlattenCmd::execute()
{
	VObject* newObject;

	VFlatten op( m_flatness );

	VObjectListIterator itr( m_selection->objects() );
	for ( ; itr.current() ; ++itr )
	{
/*		if( VGroup* group = dynamic_cast<VGroup*>( itr.current()->parent() ) )
		{
			newObject = itr.current()->clone();

			group->insertNewDeleteOld( newObject, itr.current() );
		}
		else
		{
*/
			newObject = itr.current();
//		}

		op.visit( *newObject );
	}
}

void
VFlattenCmd::unexecute()
{
}

