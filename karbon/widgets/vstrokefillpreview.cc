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

VStrokeFillPreview::VStrokeFillPreview( QWidget* parent = 0L, const char* name = 0L )
	: QFrame( parent, name )
{
	setFocusPolicy( QWidget::NoFocus );
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

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

void
VStrokeFillPreview::update( const VStroke &s, const VFill &f )
{
	// Some dummy code to show lenny how to use your own vkopainter.
	// Actually I think keeping the painter around as a member var
	// may be better. (Rob)
	kdDebug() << "VStrokeFillPreview::paintEvent" << endl;

	m_painter->begin();
	m_painter->clear( paletteBackgroundColor().rgb() );

	m_painter->setPen( Qt::NoPen );
	if( s.type() != stroke_none )
	{
		m_painter->setBrush( s.color().toQColor() );

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
		m_painter->setBrush( f.color().toQColor() );

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

