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

#include <qevent.h>
#include <qcursor.h>

#include <klocale.h>

#include "zoomtoolplugin.h"
#include <karbon_part_base.h>
#include <karbon_part.h>
#include <karbon_view_base.h>
#include <karbon_view.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <kgenericfactory.h>

typedef KGenericFactory<VZoomTool, KarbonViewBase> ZoomToolPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_zoomtoolplugin, ZoomToolPluginFactory( "karbonzoomtoolplugin" ) );

VZoomTool::VZoomTool( KarbonViewBase* view, const char *name, const QStringList & )
	: VTool( (KarbonView *)view, name ), VKarbonPlugin( view, name )
{
	registerTool( this );
}

VZoomTool::~VZoomTool()
{
}

QString
VZoomTool::contextHelp()
{
	QString s = i18n( "<qt><b>Zoom tool:</b><br>" );
	s += i18n( "<i>Click and drag</i> to zoom into a rectangular area.<br>" );
	s += i18n( "<i>Zoom using arrow keys</i><br>Zoom into/out of canvas using arrow up/down." );
	return s;
}

void
VZoomTool::activate()
{
	view()->setCursor( QCursor( Qt::crossCursor ) );
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
VZoomTool::mouseButtonRelease()
{
	/*double viewportX = view()->canvasWidget()->visibleWidth() * 0.75 / view()->zoom();
	double viewportY = view()->canvasWidget()->visibleHeight() * 0.75 / view()->zoom();
	KoRect rect( last().x() - viewportX / 2.0, last().y() - viewportY / 2.0, viewportX, viewportY );
	rect = rect.normalize();
	view()->canvasWidget()->setViewportRect( rect );*/
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
	//view()->setViewportRect( rect );
}

void
VZoomTool::arrowKeyReleased( Qt::Key key )
{
	double zoomChange = 0;
	if( key == Qt::Key_Up )
		zoomChange = 0.75;
	else if( key == Qt::Key_Down )
		zoomChange = 1.50;

	if( zoomChange != 0 )
		view()->setZoom( view()->zoom() * zoomChange );
}

void
VZoomTool::recalc()
{
	m_current = last();
}

