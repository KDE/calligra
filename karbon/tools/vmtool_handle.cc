/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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
drawBox( QPainter& painter, int cx, int cy, const double zoomFactor )
{
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	painter.drawRect( QRect( QPoint( cx - 2 / zoomFactor, cy - 2 / zoomFactor ),
							 QPoint( cx + 2 / zoomFactor, cy + 2 / zoomFactor ) ) );
}

void
VMToolHandle::draw( QPainter& painter, const double zoomFactor )
{
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );
	QWMatrix mat = painter.worldMatrix();
	painter.setWorldMatrix( mat.scale( zoomFactor, zoomFactor ) );

	if( part()->selection().count() > 0 )
	{
		QRect bbox =  part()->selection().boundingBox( 1 );
		painter.drawRect( bbox );

		// draw boxes
		drawBox( painter, bbox.left(), bbox.top(), zoomFactor );
		drawBox( painter, bbox.left() + bbox.width() / 2, bbox.top(), zoomFactor );
		drawBox( painter, bbox.right(), bbox.top(), zoomFactor );
		drawBox( painter, bbox.right(), bbox.top() + bbox.height() / 2, zoomFactor );
		drawBox( painter, bbox.right(), bbox.bottom(), zoomFactor );
		drawBox( painter, bbox.left() + bbox.width() / 2, bbox.bottom(), zoomFactor );
		drawBox( painter, bbox.left(), bbox.bottom(), zoomFactor );
		drawBox( painter, bbox.left(), bbox.top() + bbox.height() / 2, zoomFactor );
	}
}

bool
VMToolHandle::eventFilter( KarbonView* view, QEvent* event )
{
	return false;
}
