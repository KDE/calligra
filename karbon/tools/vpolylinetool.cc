/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
#include <qlabel.h>

#include <klocale.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcolor.h"
#include "vcomposite.h"
#include "vfill.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpolylinetool.h"
#include "vshapecmd.h"

// MUST BE LAST !
#include <qwindowdefs.h>
#include <X11/Xlib.h>

VPolylineTool::VPolylineTool( KarbonView* view )
	: VTool( view )
{
	m_bezierPoints.setAutoDelete( true );
}

VPolylineTool::~VPolylineTool()
{
}

QString
VPolylineTool::contextHelp()
{
	QString s = i18n( "<qt><b>Polyline tool:</b><br>" );
	s += i18n( "- <i>Click</i> to add a node and <i>drag</i> to set its bezier vector.<br>" );
	s += i18n( "- Press <i>Ctrl</i> while dragging to edit the previous bezier vector.<br>" );
	s += i18n( "- Press <i>Shift</i> while dragging to change the curve in a straight line.<br>" );
	s += i18n( "- Press <i>Backspace</i> to cancel the last curve.<br>" );
	s += i18n( "- Press <i>Esc</i> to cancel the whole polyline.<br>" );
	s += i18n( "- Press <i>Enter</i> or <i>double click</i> to end the polyline.</qt>" );

	return s;
}

void
VPolylineTool::activate()
{
	view()->statusMessage()->setText( i18n( "Polyline Tool" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );

	m_bezierPoints.clear();
	m_close = false;
}

void
VPolylineTool::deactivate()
{
	m_bezierPoints.removeLast();
	m_bezierPoints.removeLast();
	
	VComposite* polyline = 0L;
	if( m_bezierPoints.count() > 2 )
	{
		polyline = new VComposite( 0L );
		KoPoint* p1 = m_bezierPoints.first();
		KoPoint* p2;
		KoPoint* p3;
		KoPoint* p4;

		polyline->moveTo( *p1 );

		while(
			( p2 = m_bezierPoints.next() ) &&
			( p3 = m_bezierPoints.next() ) &&
			( p4 = m_bezierPoints.next() ) )
		{
			if ( *p1 == *p2 )
				if ( *p3 == *p4 )
					polyline->lineTo( *p4 );
				else
					polyline->curve1To( *p3, *p4 );
			else
				if ( *p3 == *p4 )
					polyline->curve2To( *p2, *p4 );
				else
					polyline->curveTo( *p2, *p3, *p4 );
			p1 = p4;
		}

		if( m_close )
			polyline->close();
	}

	if( polyline )
	{
		VShapeCmd* cmd = new VShapeCmd(
			&view()->part()->document(),
			i18n( "Polyline" ),
			polyline,
			"14_polyline" );

		view()->part()->addCommand( cmd, true );
		view()->selectionChanged();
	}
}

void
VPolylineTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	if( m_bezierPoints.count() > 2 )
	{
		VComposite polyline( 0L );
		polyline.moveTo( *m_bezierPoints.first() );

		KoPoint* p2;
		KoPoint *p3;
		KoPoint *p4;

		while(
			( p2 = m_bezierPoints.next() ) &&
			( p3 = m_bezierPoints.next() ) &&
			( p4 = m_bezierPoints.next() ) )
		{
			polyline.curveTo( *p2, *p3, *p4 );
		}

		polyline.setState( VObject::edit );
		polyline.draw( painter, &polyline.boundingBox() );
	}
}

void
VPolylineTool::drawBezierVector( KoPoint& start, KoPoint& end )
{
	VPainter* painter = view()->painterFactory()->editpainter();

	painter->save();
	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );

	float zoomFactor = view()->zoom();

	painter->setRasterOp( Qt::NotROP );
	painter->newPath();
/*  VStroke stroke( Qt::blue, 0L, 1.0 );
	QValueList<float> array;
	array << 2.0 << 3.0;
	stroke.dashPattern().setArray( array );*/
	painter->setPen( Qt::DotLine /*stroke*/ );
	painter->setBrush( Qt::NoBrush );

	painter->moveTo( start ); 
	painter->lineTo( end );
	painter->strokePath();
	painter->setRasterOp( Qt::XorROP );
	painter->newPath();
	painter->setPen( Qt::yellow );

	float width = 2.0;

	painter->moveTo( KoPoint(
		end.x() - width / zoomFactor,
		end.y() - width / zoomFactor ) );
	painter->lineTo( KoPoint(
		end.x() + width / zoomFactor,
		end.y() - width / zoomFactor ) );
	painter->lineTo( KoPoint(
		end.x() + width / zoomFactor,
		end.y() + width / zoomFactor ) );
	painter->lineTo( KoPoint(
		end.x() - width / zoomFactor,
		end.y() + width / zoomFactor ) );
	painter->lineTo( KoPoint(
		end.x() - width / zoomFactor,
		end.y() - width / zoomFactor ) );

	painter->strokePath();
	painter->restore();
}

void
VPolylineTool::mouseMove()
{
	if( m_bezierPoints.count() != 0 )
	{
		draw();

		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( last() ) );
		m_bezierPoints.append( new KoPoint( last() ) );

		draw();
	}
}

void
VPolylineTool::mouseButtonPress()
{
	m_shiftPressed = m_ctrlPressed = false;

	if( m_bezierPoints.count() != 0 )
	{
		draw();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( last() ) );
	}

	m_lastVectorEnd = m_lastVectorStart = last();

	m_bezierPoints.append( new KoPoint( last() ) );
	m_bezierPoints.append( new KoPoint( last() ) );
	
	drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
	draw();
}

void
VPolylineTool::mouseButtonRelease()
{
	if( m_bezierPoints.count() == 2 )
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );

		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint(last() ) );    

		VPainter* painter = view()->painterFactory()->editpainter();
		painter->save();
		view()->canvasWidget()->setYMirroring( true );
		painter->setZoomFactor( view()->zoom() );
		painter->setRasterOp( Qt::XorROP );
		VStroke stroke( Qt::yellow, 0L, 1.0 );
		painter->setPen( stroke );
		painter->setBrush( Qt::yellow );
		painter->newPath();
		painter->drawNode( m_lastVectorStart, 2 );
		painter->strokePath();
		painter->restore();
	}
	else
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
		draw();

		m_bezierPoints.removeLast();
		KoPoint* p = m_bezierPoints.last();
		m_bezierPoints.removeLast();
		KoPoint* b = m_bezierPoints.last();
		m_bezierPoints.removeLast();

		if( m_shiftPressed )
		{
			m_bezierPoints.removeLast();
			m_bezierPoints.append( new KoPoint( *m_bezierPoints.last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_lastVectorStart = m_lastVectorEnd = *p;
		}
		else if( m_ctrlPressed )
		{
			m_bezierPoints.removeLast();
			m_lastVectorStart = *m_bezierPoints.last();
			m_bezierPoints.append( new KoPoint( last() ) );
			m_bezierPoints.append( new KoPoint( *b ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			m_lastVectorEnd = last();
		}
		else
		{
			m_bezierPoints.append( new KoPoint( last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( last() - *p ) ) );
			m_lastVectorStart = *p;
			m_lastVectorEnd = last();
		}
		if( m_bezierPoints.count() > 2 && p->isNear( *m_bezierPoints.first(), 3 ) )
		{
			m_bezierPoints.append( new KoPoint( last() ) );
			m_close = true;
			accept();
			return;
		}
	}

	m_bezierPoints.append( new KoPoint( last() ) );
	m_bezierPoints.append( new KoPoint( last() ) );

	draw();
}

void
VPolylineTool::mouseButtonDblClick()
{
	accept();
}

void
VPolylineTool::mouseDrag()
{
	if( m_bezierPoints.count() == 2 )
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );

		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( last() ) );    
		m_lastVectorEnd = last();

		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
	}
	else
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
		draw();

		m_bezierPoints.removeLast();
		KoPoint* p = m_bezierPoints.last();
		m_bezierPoints.removeLast();
		KoPoint* b = m_bezierPoints.last();
		m_bezierPoints.removeLast();

		if( m_shiftPressed )
		{
			m_bezierPoints.removeLast();
			m_bezierPoints.append( new KoPoint( *m_bezierPoints.last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_lastVectorStart = m_lastVectorEnd = *p;
		}
		else if( m_ctrlPressed )
		{
			m_bezierPoints.removeLast();
			m_lastVectorStart = *m_bezierPoints.last();
			m_bezierPoints.append( new KoPoint( last() ) );
			m_bezierPoints.append( new KoPoint( *b ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			m_lastVectorEnd = last();
		}
		else
		{
			m_bezierPoints.append( new KoPoint( last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( last() - *p ) ) );
			m_lastVectorStart = *p;
			m_lastVectorEnd = last();
		}

		draw();
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
	}
}

void
VPolylineTool::mouseDragRelease()
{
	mouseButtonRelease();
}

void
VPolylineTool::mouseDragShiftPressed()
{
	m_shiftPressed = true;
	// Emits a mouse move event. Sorry...
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, 1, 1 );
}

void
VPolylineTool::mouseDragCtrlPressed()
{
	// Moves the mouse to the other bezier vector position.
	KoPoint p = *m_bezierPoints.at( m_bezierPoints.count() - 4) - *m_bezierPoints.at( m_bezierPoints.count() - 3 );
	int x = (int)( p.x() * view()->zoom() );
	int y = (int)( p.y() * view()->zoom() );
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );

	m_ctrlPressed = true;
}

void
VPolylineTool::mouseDragShiftReleased()
{
	m_shiftPressed = false;
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, -1, -1 );
}

void
VPolylineTool::mouseDragCtrlReleased()
{
	KoPoint p = *m_bezierPoints.at( m_bezierPoints.count() - 3) - *m_bezierPoints.at( m_bezierPoints.count() - 4 );
	int x = (int)( p.x() * view()->zoom() );
	int y = (int)( p.y() * view()->zoom() );
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );

	m_ctrlPressed = false;
}

void
VPolylineTool::cancel()
{
	draw();

	m_bezierPoints.clear();
}

void
VPolylineTool::cancelStep()
{
	draw();

	if ( m_bezierPoints.count() > 6 )
	{
		KoPoint p2 = *m_bezierPoints.last();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		KoPoint p1 = *m_bezierPoints.last();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( p1 ) );
		m_bezierPoints.append( new KoPoint( p1 ) );
		KoPoint p = p1 - p2;
		int x = (int)( p.x() * view()->zoom() );
		int y = (int)( p.y() * view()->zoom() );
		XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );
	}
	else
	{
		m_bezierPoints.clear();
	}

	draw();
}

void
VPolylineTool::accept()
{
	deactivate();
	activate();
}

