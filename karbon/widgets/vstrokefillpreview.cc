/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vstrokefillpreview.h"
#include "vkopainter.h"
#include "koPoint.h"
#include "kdebug.h"
#include <qcolor.h>
#include "vstroke.h"
#include "vfill.h"
#include "vfilldlg.h"
#include "vstrokedlg.h"
#include "vselection.h"
#include "karbon_part.h"
#include <kdebug.h>

VStrokeFillPreview::VStrokeFillPreview( KarbonPart *part, QWidget* parent = 0L, const char* name = 0L )
	: QFrame( parent, name ), m_part( part )
{
	setFocusPolicy( QWidget::NoFocus );
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	installEventFilter( this );

	m_pixmap.resize( 50, 50 );
	m_painter = new VKoPainter( &m_pixmap, 50, 50 );
}

VStrokeFillPreview::~VStrokeFillPreview()
{
	delete( m_painter );
}

void
VStrokeFillPreview::paintEvent( QPaintEvent* event )
{
	bitBlt( this, 0, 0, &m_pixmap, 0, 0, 50, 50 );
}

bool
VStrokeFillPreview::eventFilter( QObject *, QEvent *event )
{
	if( event && event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent *e = static_cast<QMouseEvent *>( event );
		if( e->x() >= 20 && e->x() <= 40 && e->y() >= 20 && e->y() <= 50 )
		{
			VFillDlg* dialog = new VFillDlg( m_part );
			dialog->exec();
			delete dialog;
			update( *m_part->document().selection()->objects().getFirst()->stroke(),
					*m_part->document().selection()->objects().getFirst()->fill() );
		}
		else if( e->x() >= 10 && e->x() <= 30 && e->y() >= 10 && e->y() <= 40 )
		{
			VStrokeDlg* dialog = new VStrokeDlg( m_part );
			dialog->exec();
			delete dialog;
			update( *m_part->document().selection()->objects().getFirst()->stroke(),
					*m_part->document().selection()->objects().getFirst()->fill() );
		}
	}
	return false;
}


void
VStrokeFillPreview::update( const VStroke &s, const VFill &f )
{
	m_painter->begin();
	m_painter->clear( paletteBackgroundColor().rgb() );

	m_painter->setPen( Qt::NoPen );
	if( s.type() != stroke_none )
	{
		if( s.type() == stroke_gradient )
		{
			VFill fill;
			fill.gradient() = s.gradient();
			if( s.gradient().type() == gradient_linear )
			{
				fill.gradient().setOrigin( KoPoint( 20, 10 ) );
				fill.gradient().setVector( KoPoint( 20, 40 ) );
			}
			else if( s.gradient().type() == gradient_radial )
			{
				fill.gradient().setOrigin( KoPoint( 20, 25 ) );
				fill.gradient().setVector( KoPoint( 20, 40 ) );
			}
			fill.setType( fill_gradient );
			m_painter->setBrush( fill );
		}
		else
			m_painter->setBrush( s.color() );

		m_painter->newPath();
		m_painter->moveTo( KoPoint( 10.0, 10.0 ) );
		m_painter->lineTo( KoPoint( 30.0, 10.0 ) );
		m_painter->lineTo( KoPoint( 30.0, 40.0 ) );
		m_painter->lineTo( KoPoint( 10.0, 40.0 ) );
		m_painter->lineTo( KoPoint( 10.0, 10.0 ) );
		m_painter->fillPath();
	}

	if( f.type() != fill_none )
	{
		if( f.type() == fill_gradient )
		{
			VFill fill;
			fill = f;
			if( f.gradient().type() == gradient_linear )
			{
				fill.gradient().setOrigin( KoPoint( 30, 20 ) );
				fill.gradient().setVector( KoPoint( 30, 50 ) );
			}
			else if( f.gradient().type() == gradient_radial )
			{
				fill.gradient().setOrigin( KoPoint( 30, 35 ) );
				fill.gradient().setVector( KoPoint( 30, 50 ) );
			}
			m_painter->setBrush( fill );
		}
		else
			m_painter->setBrush( f.color() );

		m_painter->newPath();
		m_painter->moveTo( KoPoint( 20.0, 20.0 ) );
		m_painter->lineTo( KoPoint( 40.0, 20.0 ) );
		m_painter->lineTo( KoPoint( 40.0, 50.0 ) );
		m_painter->lineTo( KoPoint( 20.0, 50.0 ) );
		m_painter->lineTo( KoPoint( 20.0, 20.0 ) );
		m_painter->fillPath();
	}
	m_painter->end();

	repaint();
}

#include "vstrokefillpreview.moc"

