/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vgradienttool.h"
#include "vgradientdlg.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"


VGradientTool::VGradientTool( KarbonView* view )
	: VTool( view )
{
	m_isDragging = false;
	m_dialog = new VGradientDlg();
	m_dialog->setGradientRepeat( VGradient::none );
	m_dialog->setGradientType( VGradient::linear );
	m_dialog->setGradientFill( 1 );
	m_dialog->setStartColor( Qt::red );
	m_dialog->setEndColor( Qt::yellow );
}

VGradientTool::~VGradientTool()
{
	delete m_dialog;
}

void
VGradientTool::mouseReleased( QMouseEvent *mouse_event )
{
	if( !m_isDragging ) return;

	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
	KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );

	VGradient gradient;
	gradient.clearStops();
	gradient.addStop( VColor( m_dialog->startColor().rgb() ), 0.0, 0.5 );
	gradient.addStop( VColor( m_dialog->endColor().rgb() ), 1.0, 0.5 );
	gradient.setOrigin( fp * ( 1.0 / view()->zoom() ) );
	gradient.setVector( lp * ( 1.0 / view()->zoom() ) );
	gradient.setType( (VGradient::VGradientType)m_dialog->gradientType() );
	gradient.setRepeatMethod( (VGradient::VGradientRepeatMethod)m_dialog->gradientRepeat() );

	if( m_dialog->gradientFill() )
	{
		VFill fill;
		fill.gradient() = gradient;
		fill.setType( VFill::grad );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill ), true );
	}
	else
	{
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &gradient ), true );
	}

	view()->selectionChanged();

	m_isDragging = false;
}

void
VGradientTool::mousePressed( QMouseEvent *mouse_event )
{
	view()->painterFactory()->painter()->end();

	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	// draw initial object:
	drawTemporaryObject();
	m_isDragging = true;
}

void
VGradientTool::activate()
{
	view()->statusMessage()->setText( i18n( "Gradient" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

void
VGradientTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	painter->moveTo( KoPoint( m_lp.x(), m_lp.y() ) );
	painter->lineTo( KoPoint( m_fp.x(), m_fp.y() ) );
	painter->strokePath();
}

void
VGradientTool::showDialog() const
{
	m_dialog->exec();
}

