/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vstrokefillpreview.h"
#include "vkopainter.h"
#include "koPoint.h"
#include "kdebug.h"

VStrokeFillPreview::VStrokeFillPreview( QWidget* parent = 0L, const char* name = 0L )
	: QFrame( parent, name )
{
	setFocusPolicy( QWidget::NoFocus );
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

	m_painter = new VKoPainter( this, 50, 50 );
}

VStrokeFillPreview::~VStrokeFillPreview()
{
	delete( m_painter );
}

void VStrokeFillPreview::paintEvent( QPaintEvent* /*event*/ )
{
	// Some dummy code to show lenny how to use your own vkopainter.
	// Actually I think keeping the painter around as a member var
	// may be better. (Rob)
	kdDebug() << "VStrokeFillPreview::paintEvent" << endl;

	m_painter->begin();

	m_painter->setPen( Qt::black );

	m_painter->newPath();
	m_painter->moveTo( KoPoint( 10.0, 10.0 ) );
	m_painter->lineTo( KoPoint( 30.0, 10.0 ) );
	m_painter->lineTo( KoPoint( 30.0, 40.0 ) );
	m_painter->lineTo( KoPoint( 10.0, 40.0 ) );
	m_painter->lineTo( KoPoint( 10.0, 10.0 ) );
	m_painter->strokePath();

	m_painter->end();
}

#include "vstrokefillpreview.moc"

