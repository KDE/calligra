/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vhandle.h"
#include "vobject.h"
#include <kdebug.h>

#include <qpainter.h>

VHandle::VHandle()
{
}

VHandle::~VHandle()
{
}

void
VHandle::draw( QPainter& painter )
{
	// calculate bbox
	QRect rect;
	QPtrListIterator<VObject> itr = m_objects;
	if( !itr.current() )
		return;
	else
		rect = itr.current()->boundingBox();
	++itr;

	for ( ; itr.current() ; ++itr )
	{
		rect = rect.unite( itr.current()->boundingBox() );
	}

	// Draw selection handle
	// TODO : selection -> handle, editing -> nodes/boxes ?
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );
	painter.drawRect( rect );

	// draw boxes
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	drawBox( painter, rect.left(), rect.top() );
	drawBox( painter, rect.left() + rect.width() / 2, rect.top() );
	drawBox( painter, rect.right(), rect.top() );
	drawBox( painter, rect.right(), rect.top() + rect.height() / 2 );
	drawBox( painter, rect.right(), rect.bottom() );
	drawBox( painter, rect.left() + rect.width() / 2, rect.bottom() );
	drawBox( painter, rect.left(), rect.bottom() );
	drawBox( painter, rect.left(), rect.top() + rect.height() / 2 );
}

void
VHandle::addObject( const VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
}

void
VHandle::reset()
{
	m_objects.clear();
}

void
VHandle::drawBox( QPainter& painter, double x, double y, uint handleSize )
{
    painter.drawRect( x - handleSize, y - handleSize,
	                      handleSize*2 + 1, handleSize*2 + 1 );
}
