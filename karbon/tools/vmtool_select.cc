/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vmtool_select.h"
#include "vmcmd_select.h"
#include "vmcmd_transform.h"
#include "vhandle.h"

#include <kdebug.h>

VMToolSelect* VMToolSelect::s_instance = 0L;

VMToolSelect::VMToolSelect( KarbonPart* part )
	: VMTool( part )
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
	QPainter painter( view->canvasWidget()->viewport() );
	// use p2 for content coord instead of viewport coord
	QPoint p2 = view->canvasWidget()->contentsToViewport( p );
	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->handle()->objects().isEmpty() &&
		part()->handle()->boundingBox().contains( p / view->zoomFactor() ) )
	{
		if( m_TransformState != Moving )
			m_TransformState = Moving;
		QRect rect = part()->handle()->boundingBox();
		//rect.moveTopLeft( view->canvasWidget()->contentsToViewport( rect.topLeft() ) );
		kdDebug() << "p.x() : " << p.x() << endl;
		kdDebug() << "p.y() : " << p.y() << endl;
		kdDebug() << "d1 : " << d1 << endl;
		kdDebug() << "d2 : " << d2 << endl;
		kdDebug() << "rect.y() : " << rect.y() << endl;
		kdDebug() << "rect.y() : " << rect.y() << endl;
		kdDebug() << "rect.width() : " << rect.width() << endl;
		kdDebug() << "rect.height() : " << rect.height() << endl;
		painter.setRasterOp( Qt::NotROP );
		painter.translate( -view->canvasWidget()->contentsX(),
							-view->canvasWidget()->contentsY() );
		painter.translate( d1, d2 );
		//painter.scale( d1 / double( rect.width() / 2 ), d2 / double( rect.height() / 2 ) );
		kdDebug() << "Middle x : " << ( p.x() + d1 ) - ( rect.x() + rect.width() / 2 ) << endl;
		kdDebug() << "Middle y : " << ( p.y() + d2 ) - ( rect.y() + rect.height() / 2 ) << endl;
		QPtrList<VObject> objects = part()->handle()->objects();
		QPtrListIterator<VObject> itr( objects );
		for ( ; itr.current() ; ++itr )
			itr.current()->draw( painter, rect, view->zoomFactor() );
		part()->drawHandle( painter, view->zoomFactor() );
	}
	else
	{
		painter.save();
		painter.setPen( Qt::DotLine );
		painter.setRasterOp( Qt::NotROP );

		painter.moveTo( p2.x(), p2.y() );
		painter.lineTo( p2.x() + d1, p2.y() );
		painter.lineTo( p2.x() + d1, p2.y() + d2 );
		painter.lineTo( p2.x(), p2.y() + d2 );
		painter.lineTo( p2.x(), p2.y() );

		painter.restore();
	}
}

VCommand*
VMToolSelect::createCmd( double x, double y, double d1, double d2 )
{
	if( m_TransformState == Moving )
	{
		QWMatrix mat;
		QRect rect = part()->handle()->boundingBox();
		mat.translate( d1, d2 );
		m_TransformState = NoTransform;
		return
			new VMCmdTransform( part(), part()->handle()->objects(), mat );
	}
	else
	{
		// we dont want select to be undoable
		part()->unselectObjects();
		VMCmdSelect cmd( part(), x, y, x + d1, y + d2 );
		cmd.execute();
		return 0L;
	}
}

void
VMToolSelect::startDragging()
{
}

