/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vmtool_select.h"
#include "vmcmd_select.h"

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
	painter.save();
	painter.setPen( Qt::DotLine );
	painter.setRasterOp( Qt::NotROP );

	painter.moveTo( p.x(), p.y() );
	painter.lineTo( p.x() + d1, p.y() );
	painter.lineTo( p.x() + d1, p.y() + d2 );
	painter.lineTo( p.x(), p.y() + d2 );
	painter.lineTo( p.x(), p.y() );

	painter.restore();
}

VCommand*
VMToolSelect::createCmd( const QPoint& p, double d1, double d2 )
{
	return
		new VMCmdSelect( part(),
			p.x(), p.y(),
			p.x() + d1,
			p.y() + d2 );
}

