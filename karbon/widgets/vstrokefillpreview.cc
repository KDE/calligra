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
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
}

void VStrokeFillPreview::paintEvent( QPaintEvent* /*event*/ )
{
	// Some dummy code to show lenny how to use your own vkopainter.
	// Actually I think keeping the painter around as a member var
	// may be better. (Rob)
	/*kdDebug() << "VStrokeFillPreview::paintEvent" << endl;
	VKoPainter p( this, 50, 50 );
	p.begin();
	p.setZoomFactor( 1.0 );
	p.setPen( Qt::yellow );

	p.newPath();
	p.moveTo( KoPoint( 10, 30 ) );
	p.lineTo( KoPoint( 30, 30 ) );
	p.strokePath();

	p.end();*/
}

#include "vstrokefillpreview.moc"

