/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vlayer.h"

VLayer::VLayer()
	: m_isVisible( true ), m_isReadOnly( false )
{
}

VLayer::~VLayer()
{
	for ( VObject* object=m_objects.first(); object!=0L; object=m_objects.next() )
	{
		delete( object );
	}
}

void
VLayer::draw( QPainter& painter, const QRect& rect, const double& zoomFactor )
{
	QListIterator<VObject> i = m_objects;
	for ( ; i.current() ; ++i )
	{
		i.current()->draw( painter, rect, zoomFactor );
	}
}
