/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_select.h"

VMToolSelect* VMToolSelect::s_instance = 0L;

VMToolSelect::VMToolSelect( KarbonPart* part )
	: VTool( part )
{
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
	painter.setRasterOp( Qt::NotROP );
	painter.setPen( Qt::DotLine );

	painter.moveTo( p.x(), p.y() );
	painter.lineTo( p.x() + d1, p.y() );
	painter.lineTo( p.x() + d1, p.y() + d2 );
	painter.lineTo( p.x(), p.y() + d2 );
	painter.lineTo( p.x(), p.y() );
}

VCommand*
VMToolSelect::createCmd( double x, double y, double d1, double d2 )
{
// TODO: swap coords to optimize normalize() away
	part()->selectObjectsWithinRect(
		KoRect( x, y, x + d1, y + d2 ).normalize(), true );

	return 0L;
}
