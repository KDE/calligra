/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <klocale.h>

#include "vorderselectioncmd.h"
#include "vselection.h"

VOrderSelectionCmd::VOrderSelectionCmd( VDocument *doc, VOrder state )
	: VCommand( doc, i18n( "Order Selection" ) ), m_state( state )
{
	m_selection = m_doc->selection()->clone();
}

VOrderSelectionCmd::~VOrderSelectionCmd()
{
	delete( m_selection );
}

void
VOrderSelectionCmd::execute()
{
	if( m_state == sendtoback )
	{
		VObjectListIterator itr( m_doc->selection()->objects() );
		for ( itr.toLast() ; itr.current() ; --itr )
		{
			// remove from old layer
			VObjectList objects;
			VLayerListIterator litr( m_doc->layers() );

			for ( ; litr.current(); ++litr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
						litr.current()->sendToBack( *itr2.current() );
			}
		}
	}
	else if( m_state == bringtofront )
	{
		VObjectListIterator itr( m_doc->selection()->objects() );
		for ( ; itr.current() ; ++itr )
		{
			// remove from old layer
			VObjectList objects;
			VLayerListIterator litr( m_doc->layers() );

			for ( ; litr.current(); ++litr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
						litr.current()->bringToFront( *itr2.current() );
			}
		}
	}
	else if( m_state == up || m_state == down )
	{
		VSelection selection = *m_doc->selection();

		VObjectList objects;

		VLayerListIterator litr( m_doc->layers() );
		while( !selection.objects().isEmpty() )
		{
			for ( ; litr.current(); ++litr )
			{
				VObjectList todo;
				VObjectListIterator itr( selection.objects() );
				for ( ; itr.current() ; ++itr )
				{
					objects = litr.current()->objects();
					VObjectListIterator itr2( objects );
					// find all selected VObjects that are in the current layer
					for ( ; itr2.current(); ++itr2 )
					{
						if( itr2.current() == itr.current() )
						{
							todo.append( itr.current() );
							// remove from selection
							selection.take( *itr.current() );
						}
					}
				}

				// we have found the affected vobjects in this vlayer
				VObjectListIterator itr3( todo );
				for ( ; itr3.current(); ++itr3 )
				{
					if( m_state == up )
						litr.current()->upwards( itr3.current() );
					else
						litr.current()->downwards( itr3.current() );
				}
			}
		}
	}
}

void
VOrderSelectionCmd::unexecute()
{
}
