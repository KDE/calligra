/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_handle.h"

#include <kdebug.h>

VMToolHandle* VMToolHandle::s_instance = 0L;

VMToolHandle::VMToolHandle( KarbonPart* part )
	: VTool( part )
{
}

VMToolHandle::~VMToolHandle()
{
}

VMToolHandle*
VMToolHandle::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolHandle( part );
	}

	return s_instance;
}

void
VMToolHandle::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
/*
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setRasterOp( Qt::NotROP );
	painter.setPen( Qt::DotLine );
	painter.drawRect( p.x(), p.y(), d1, d2 );
	*/
}

VCommand*
VMToolHandle::createCmd( double x, double y, double d1, double d2 )
{
/*
	part()->unselectAllObjects();

	part()->selectObjectsWithinRect(
		KoRect( x, y, x + d1, y + d2 ) );
*/
	return 0L;
}
/*
void
VMToolHandle::draw( QPainter& painter, const double zoomFactor )
{
	// calculate bbox
	QPtrListIterator<VObject> itr = m_objects;
	if( !itr.current() )
		return;
	else
		m_bbox = itr.current()->boundingBox( zoomFactor );
	++itr;

	for ( ; itr.current() ; ++itr )
	{
		m_bbox = m_bbox.unite( itr.current()->boundingBox( zoomFactor ) );
	}

	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );

	// for now I think this is enough, maybe we need something else when
	// handle is rotated ? Then again, this is just a simple rect.
	//painter.scale( zoomFactor, zoomFactor );

	// Draw selection handle
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
VMToolHandle::addObject( const VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
}

void
VMToolHandle::removeObject( const VObject* object )
{
	m_objects.remove( object );
}

void
VMToolHandle::deleteObjects( QPtrList<VObject> &objects )
{
	QPtrListIterator<VObject> itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->setState( VObject::deleted );

	objects = m_objects;
	m_bbox = QRect();
	reset();
}

void
VMToolHandle::undeleteObjects( QPtrList<VObject> &objects )
{
	QPtrListIterator<VObject> itr = objects;
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( VObject::normal );
		addObject( itr.current() );
	}
}

void
VMToolHandle::reset()
{
	m_objects.clear();
}

void
VMToolHandle::drawBox( QPainter& painter, double x, double y, uint handleSize )
{
    painter.drawRect( x - handleSize, y - handleSize,
	                      handleSize*2 + 1, handleSize*2 + 1 );
}

*/
