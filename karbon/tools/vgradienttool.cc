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
	: VTool( view ), m_isDragging( false )
{
	m_dialog = new VGradientDlg();
	m_dialog->setGradientRepeat( gradient_repeat_none );
	m_dialog->setGradientType( gradient_linear );
	m_dialog->setGradientFill( 1 );
	m_dialog->setStartColor( Qt::red );
	m_dialog->setEndColor( Qt::yellow );
}

VGradientTool::~VGradientTool()
{
	delete m_dialog;
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

bool
VGradientTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );

	if ( event->type() == QEvent::MouseMove )
	{
		if( m_isDragging )
		{
			// erase old object:
			drawTemporaryObject();

			m_lp.setX( mouse_event->pos().x() );
			m_lp.setY( mouse_event->pos().y() );

			// paint new object:
			drawTemporaryObject();
		}

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
		KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );

		VGradient gradient;
		gradient.addStop( VColor( m_dialog->startColor().rgb() ), 0.0, 0.5 );
		gradient.addStop( VColor( m_dialog->endColor().rgb() ), 1.0, 0.5 );
		gradient.setOrigin( fp * ( 1.0 / view()->zoom() ) );
		gradient.setVector( lp * ( 1.0 / view()->zoom() ) );
		gradient.setType( (VGradientType)m_dialog->gradientType() );
		gradient.setRepeatMethod( (VGradientRepeatMethod)m_dialog->gradientRepeat() );

		if( m_dialog->gradientFill() )
		{
			VFill fill;
			fill.gradient() = gradient;
			fill.setType( fill_gradient );
			view()->part()->addCommand( new VFillCmd( &view()->part()->document(), fill ), true );
		}
		else
		{
			VStroke stroke;
			stroke.gradient() = gradient;
			stroke.setType( stroke_gradient );
			view()->part()->addCommand( new VStrokeCmd( &view()->part()->document(), stroke ), true );
		}

		m_isDragging = false;

		return true;
	}

	// handle pressing of keys:
	if ( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>( event );

		// cancel dragging with ESC-key:
		if ( key_event->key() == Qt::Key_Escape && m_isDragging )
		{
			m_isDragging = false;

			// erase old object:
			drawTemporaryObject();

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		view()->painterFactory()->painter()->end();

		m_fp.setX( mouse_event->pos().x() );
		m_fp.setY( mouse_event->pos().y() );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// draw initial object:
		drawTemporaryObject();
		m_isDragging = true;

		return true;
	}

	return false;
}

void
VGradientTool::showDialog() const
{
	m_dialog->exec();
}


