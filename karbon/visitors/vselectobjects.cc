/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/


#include "vselectobjects.h"


void
VSelectObjects::visitVObject( VObject& object )
{
	// never select a deleted object
	if( object.state() == VObject::deleted ) return;
	if( !m_rect.isEmpty() )
	{
		if( m_select )
		{
			if( m_rect.intersects( object.boundingBox() ) )
			{
				object.setState( VObject::selected );
				m_selection.append( &object );
				setSuccess();
			}
		}
		else
		{
			if( m_rect.intersects( object.boundingBox() ) )
			{
				object.setState( VObject::normal );
				m_selection.clear();
				setSuccess();
			}
		}
	}
	else
	{
		if( m_select )
		{
			object.setState( VObject::selected );
			m_selection.append( &object );
			setSuccess();
		}
		else
		{
			object.setState( VObject::normal );
			setSuccess();
		}
	}
}

