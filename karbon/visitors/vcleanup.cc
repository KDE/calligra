/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vcleanup.h"
#include "vlayer.h"

void
VCleanUp::visitVLayer( VLayer& layer )
{
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->state() == VObject::deleted )
		{
			delete( itr.current() );
			layer.take( *itr.current() );
		}
	}
}

