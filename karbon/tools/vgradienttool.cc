/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vgradienttool.h"
#include "vgradientdlg.h"
#include "vfillcmd.h"


VGradientTool* VGradientTool::s_instance = 0L;

VGradientTool::VGradientTool( KarbonPart* part )
	: VTool( part ), m_isDragging( false )
{
	m_dialog = new VGradientDlg();
	m_dialog->setGradientRepeat( 0 );
	m_dialog->setGradientType( 0 );
}

VGradientTool::~VGradientTool()
{
	delete m_dialog;
}

VGradientTool*
VGradientTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VGradientTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VGradientTool::drawTemporaryObject( KarbonView* view )
{
	kdDebug() << "VGradientTool::drawTemporaryObject" << endl;
	VPainter *painter = view->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	painter->moveTo( KoPoint( m_lp.x(), m_lp.y() ) );
	painter->lineTo( KoPoint( m_fp.x(), m_fp.y() ) );
	painter->strokePath();
}

bool
VGradientTool::eventFilter( KarbonView* view, QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );

	if ( event->type() == QEvent::MouseMove )
	{
		if( m_isDragging )
		{
			// erase old object:
			drawTemporaryObject( view );

			m_lp.setX( mouse_event->pos().x() );
			m_lp.setY( mouse_event->pos().y() );

			// paint new object:
			drawTemporaryObject( view );
		}

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		KoPoint fp = view->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );

		VFill fill;
		fill.gradient().addStop( VColor( qRgba( 255, 0, 0, 255 ) ), 0.0, 0.5 );
		fill.gradient().addStop( VColor( qRgba( 255, 255, 0, 255 ) ), 1.0, 0.5 );
		fill.gradient().setOrigin( m_fp );
		fill.gradient().setVector( m_lp );
		fill.setType( fill_gradient );
		fill.gradient().setType( (VGradientType)m_dialog->gradientType() );
		fill.gradient().setRepeatMethod( (VGradientRepeatMethod)m_dialog->gradientRepeat() );
		part()->addCommand( new VFillCmd( &part()->document(), fill ), true );

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
			drawTemporaryObject( view );

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		view->painterFactory()->painter()->end();

		m_fp.setX( mouse_event->pos().x() );
		m_fp.setY( mouse_event->pos().y() );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// draw initial object:
		drawTemporaryObject( view );
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


