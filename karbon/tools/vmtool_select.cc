/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_select.h"
#include "vmcmd_transform.h"

VMToolSelect* VMToolSelect::s_instance = 0L;

VMToolSelect::VMToolSelect( KarbonPart* part )
	: VTool( part )
{
	m_TransformState = NoTransform;
}

VMToolSelect::~VMToolSelect()
{
}

VMToolSelect*
VMToolSelect::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolSelect( part );
	}

	return s_instance;
}

void
VMToolSelect::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	m_view = view;

	QPainter painter( view->canvasWidget()->viewport() );
	painter.setRasterOp( Qt::NotROP );
   // already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty()
		&& ( m_TransformState != NoTransform ||
			part()->selection().getFirst()->boundingBox( view->zoomFactor() ).contains( p ) ) )
//		part()->selection()->boundingBox().contains( p /* view->zoomFactor() */ ) ) )
	{
		if( m_TransformState != Moving )
			m_TransformState = Moving;

		// move operation
		QWMatrix mat;
		mat.translate( d1 / view->zoomFactor(),
						d2 / view->zoomFactor() );

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = part()->selection();
		VObjectList list;
		list.setAutoDelete( true );
	    for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}
		VObjectListIterator itr2 = list;
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->transform( mat );
			itr2.current()->setState( VObject::edit );
			itr2.current()->draw(
				painter,
				itr2.current()->boundingBox( view->zoomFactor() ),
				view->zoomFactor() );
		}
	}
	else
	{
		painter.setPen( Qt::DotLine );

		painter.moveTo( p.x(), p.y() );
		painter.lineTo( p.x() + d1, p.y() );
		painter.lineTo( p.x() + d1, p.y() + d2 );
		painter.lineTo( p.x(), p.y() + d2 );
		painter.lineTo( p.x(), p.y() );

		m_TransformState = NoTransform;
	}
}

VCommand*
VMToolSelect::createCmd( double x, double y, double d1, double d2 )
{
// TODO: swap coords to optimize normalize() away
	if( m_TransformState == Moving )
	{
		m_TransformState = NoTransform;
		//QWMatrix mat;
		//mat.translate( d1, d2 );
		return
			new VMCmdTranslate( part(), part()->selection(), d1, d2 );

	}
	else
	{
		part()->selectObjectsWithinRect(
			QRect(
				qRound( m_view->zoomFactor() * x ),
				qRound( m_view->zoomFactor() * y ),
				qRound( m_view->zoomFactor() * ( d1) ),
				qRound( m_view->zoomFactor() * ( d2 ) ) ).normalize(),
			m_view->zoomFactor(),
			true );
	}

	return 0L;
}
