/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include "vstrokefillpreview.h"


VStrokeFillPreview::VStrokeFillPreview( QWidget* parent = 0L, const char* name = 0L )
	: QFrame( parent, name )
{
	setFocusPolicy( QWidget::NoFocus );
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
}

void VStrokeFillPreview::paintEvent( QPaintEvent* /*event*/ )
{
/*
	VPainter* p = m_view->painterFactory()->painter();
	p->begin();
	p->setZoomFactor( 1.0 );

	p->end();
*/
}

#include "vstrokefillpreview.moc"

