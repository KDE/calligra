/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

#include <qcursor.h>
#include <qevent.h>
#include <qpixmap.h>

#include <klocale.h>

#include "zoomtoolplugin.h"
#include <karbon_part_base.h>
#include <karbon_part.h>
#include <karbon_view_base.h>
#include <karbon_view.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <kgenericfactory.h>

static const char* const cminus[] = {
"16 16 6 1",
"  c Gray0",
". c #939393",
"X c Gray63",
"o c #aeaeae",
"O c None",
"+ c Gray100",
"OOOOo    XXoOOOO",
"OOo  ++++  XoOOO",
"OO ++++++++ XoOO",
"Oo ++++++++ XXoO",
"O ++++++++++ XoO",
"O ++      ++ XoO",
"O ++      ++ XoO",
"O ++++++++++ XoO",
"Oo ++++++++ .oOO",
"OO ++++++++ .oOO",
"OOo  ++++   .oOO",
"OOOOo    O   XoO",
"OOOOOOOOOOO   Xo",
"OOOOOOOOOOOO   X",
"OOOOOOOOOOOOO   ",
"OOOOOOOOOOOOOO  "
};

static const char* const cplus[] = {
"16 16 6 1",
"  c Gray0",
". c #939393",
"X c Gray63",
"o c #aeaeae",
"O c None",
"+ c Gray100",
"OOOo    XXoOOOOO",
"Oo  ++++  XoOOOO",
"O ++++++++ XoOOO",
"o +++  +++ XXoOO",
" ++++  ++++ XoOO",
" ++      ++ XoOO",
" ++      ++ XoOO",
" ++++  ++++ XoOO",
"o +++  +++ .oOOO",
"O ++++++++ .oOOO",
"Oo  ++++   .oOOO",
"OOOo    O   XoOO",
"OOOOOOOOOO   XoO",
"OOOOOOOOOOO   XO",
"OOOOOOOOOOOO   O",
"OOOOOOOOOOOOO  O"
};

typedef KGenericFactory<VZoomTool, KarbonViewBase> ZoomToolPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_zoomtoolplugin, ZoomToolPluginFactory( "karbonzoomtoolplugin" ) )

VZoomTool::VZoomTool( KarbonViewBase* view, const char *name, const QStringList & )
	: VTool( (KarbonPart *)view->part(), name ), VKarbonPlugin( view, name )
{
	QPixmap pix;

	pix = QPixmap( ( const char**) cminus );
	m_minusCursor = new QCursor( pix, pix.width()/2, pix.height()/2 );

	pix = QPixmap( ( const char**) cplus );
	m_plusCursor = new QCursor( pix, pix.width()/2, pix.height()/2 );

	registerTool( this );
}

VZoomTool::~VZoomTool()
{
	delete m_minusCursor;
	delete m_plusCursor;
}

QString
VZoomTool::contextHelp()
{
	QString s = i18n( "<qt><b>Zoom tool:</b><br>" );
	s += i18n( "<i>Click and drag</i> to zoom into a rectangular area.<br>" );
	s += i18n( "<i>Right click</i> to zoom out of convas.<br>" );
	s += i18n( "<i>Pressing +/- keys</i><br>to zoom into/out of canvas." );
	return s;
}

void
VZoomTool::activate()
{
	view()->setCursor( *m_plusCursor );
}

QString
VZoomTool::statusText()
{
	return i18n( "Zoom Tool" );
}

void
VZoomTool::deactivate()
{
}

void
VZoomTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	if( isDragging() )
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( first().x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), first().y() ) );
		painter->strokePath();
	}
}

void
VZoomTool::mouseButtonPress()
{
	m_current = first();

	recalc();

	draw();
}

void 
VZoomTool::rightMouseButtonRelease()
{
	view()->setZoomAt( view()->zoom() * 0.75, last() );
}

void
VZoomTool::mouseButtonRelease()
{
	view()->setZoomAt( view()->zoom() * 1.5, last() );
}

void
VZoomTool::mouseDrag()
{
	draw();

	recalc();

	draw();
}

void
VZoomTool::mouseDragRelease()
{
	KoRect rect( first().x(), first().y(), last().x() - first().x(), last().y() - first().y() );
	rect = rect.normalize();
	view()->setViewportRect( rect );
}

bool
VZoomTool::keyReleased( Qt::Key key )
{
	double zoomChange = 0;
	if( key == Qt::Key_Minus )
		zoomChange = 0.75;
	else if( key == Qt::Key_Plus )
		zoomChange = 1.50;

	if( zoomChange != 0 )
	{
		view()->setZoomAt( view()->zoom() * zoomChange );
		return true;
	}
	return false;
}

void
VZoomTool::recalc()
{
	m_current = last();
}

