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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

*/

#include <qcursor.h>
#include <qevent.h>
#include <QRectF>

#include <klocale.h>

#include "vzoomtool.h"
#include <karbon_part.h>
#include <karbon_part.h>
#include <karbon_view.h>
#include <karbon_view.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <core/vcursor.h>

#include <kactioncollection.h>

VZoomTool::VZoomTool(KarbonView *view ): VTool( view, "tool_zoom_plugin" )
{
	m_plusCursor = new QCursor( VCursor::createCursor( VCursor::ZoomPlus ) );

	registerTool( this );
}

VZoomTool::~VZoomTool()
{
	delete m_plusCursor;
}

QString
VZoomTool::contextHelp()
{
	QString s = i18n( "<qt><b>Zoom tool:</b><br>" );
	s += i18n( "<i>Click and drag</i> to zoom into a rectangular area.<br>" );
	s += i18n( "<i>Right click</i> to zoom out of canvas.<br>" );
	s += i18n( "<i>Pressing +/- keys</i><br>to zoom into/out of canvas." );
	return s;
}

void
VZoomTool::activate()
{
	VTool::activate();
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
	// TODO rasterops need porting
	// painter->setRasterOp( Qt::NotROP );

	if( isDragging() )
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
/* TODO lineto and moveto need porting
		painter->moveTo( QPointF( first().x(), first().y() ) );
		painter->lineTo( QPointF( m_current.x(), first().y() ) );
		painter->lineTo( QPointF( m_current.x(), m_current.y() ) );
		painter->lineTo( QPointF( first().x(), m_current.y() ) );
		painter->lineTo( QPointF( first().x(), first().y() ) );
*/
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
	QRectF rect( first().x(), first().y(), last().x() - first().x(), last().y() - first().y() );
	rect = rect.normalized();
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

void
VZoomTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( i18n( "Zoom Tool" ), "14_zoom", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Zoom" ) );
		// TODO needs porting: m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

