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
	QPtrListIterator<VObject> itr = m_objects;
	if( !itr.current() )
		return;
	else
		m_bbox = itr.current()->boundingBox();
	++itr;

	for ( ; itr.current() ; ++itr )
	{
		m_bbox = m_bbox.unite( itr.current()->boundingBox() );
	}

	// Draw selection handle
	// TODO : selection -> handle, editing -> nodes/boxes ?
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );
	painter.drawRect( m_bbox );

	// draw boxes
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	drawBox( painter, m_bbox.left(), m_bbox.top() );
	drawBox( painter, m_bbox.left() + m_bbox.width() / 2, m_bbox.top() );
	drawBox( painter, m_bbox.right(), m_bbox.top() );
	drawBox( painter, m_bbox.right(), m_bbox.top() + m_bbox.height() / 2 );
	drawBox( painter, m_bbox.right(), m_bbox.bottom() );
	drawBox( painter, m_bbox.left() + m_bbox.width() / 2, m_bbox.bottom() );
	drawBox( painter, m_bbox.left(), m_bbox.bottom() );
	drawBox( painter, m_bbox.left(), m_bbox.top() + m_bbox.height() / 2 );
}

void
VHandle::addObject( const VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
}

void
VHandle::removeObject( const VObject* object )
{
	m_objects.remove( object );
}

void
VHandle::deleteObjects( QPtrList<VObject> &objects )
{
	QPtrListIterator<VObject> itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->setState( VObject::deleted );

	objects = m_objects;
	m_bbox = QRect();
	reset();
}

void
VHandle::undeleteObjects( QPtrList<VObject> &objects )
{
	QPtrListIterator<VObject> itr = objects;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::normal );
		addObject( itr.current() );
	}
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
