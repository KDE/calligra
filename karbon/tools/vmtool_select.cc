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
	KarbonView* view, const QPoint& tl, const QPoint& br )
{
	QPainter painter( view->canvasWidget()->viewport() );

	painter.moveTo( tl.x(), tl.y() );
	painter.lineTo( br.x(), tl.y() );
	painter.lineTo( br.x(), br.y() );
	painter.lineTo( tl.x(), br.y() );
	painter.lineTo( tl.x(), tl.y() );
}

VCommand*
VMToolSelect::createCmdFromDialog( const QPoint& point )
{
	return 0L;
}

VCommand*
VMToolSelect::createCmdFromDragging( const QPoint& tl, const QPoint& br )
{
	return
		new VMCmdSelect(
			part(), tl.x(), tl.y(), br.x(), br.y() );
}

