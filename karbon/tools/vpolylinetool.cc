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

#include <klocale.h>
#include <qcursor.h>
#include <qevent.h>
#include <qlabel.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vshapecmd.h"
#include "vpolylinetool.h"
#include "vcomposite.h"
#include "vfill.h"
#include "vcolor.h"

	// MUST BE LAST !
#include <qwindowdefs.h>
#include <X11/Xlib.h>

VPolylineTool::VPolylineTool( KarbonView* view )
		: VTool( view )
{
	bezierPoints.setAutoDelete( true );
} // VPolylineTool::VPolylineTool

VPolylineTool::~VPolylineTool()
{
} // VPolylineTool::~VPolylineTool

QString VPolylineTool::contextHelp()
{
	QString s = i18n( "<qt><b>Polyline tool:</b><br>" );
	s += i18n( "- <i>Click</i> to add a node and <i>drag</i> to set its bezier vector.<br>" );
	s += i18n( "- Press <i>Ctrl</i> while dragging to edit the previous bezier vector.<br>" );
	s += i18n( "- Press <i>Shift</i> while dragging to change the curve in a straight line.<br>" );
	s += i18n( "- Press <i>Backspace</i> to cancel the last curve.<br>" );
	s += i18n( "- Press <i>Esc</i> to cancel the whole polyline.<br>" );
	s += i18n( "- Press <i>Enter</i> or <i>double click</i> to end the polyline.</qt>" );
	return s;
} // VPolylineTool::contextHelp

void VPolylineTool::doActivate()
{
	view()->statusMessage()->setText( i18n("Polyline") );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );

	bezierPoints.clear();
	m_close = false;
} // VPolylineTool::doActivate

void VPolylineTool::deactivate()
{
	//draw();
	
	bezierPoints.removeLast();
	bezierPoints.removeLast();
	
	VComposite* polyline = 0L;
	if(bezierPoints.count() > 2 )
	{
		polyline = new VComposite( 0L );
		polyline->moveTo( *bezierPoints.first() );
		KoPoint *p2, *p3, *p4;
		while( ( p2 = bezierPoints.next() ) && ( p3 = bezierPoints.next() ) && ( p4 = bezierPoints.next() ) )
		{
			polyline->curveTo( *p2, *p3, *p4 );
		}
		if ( m_close )
			polyline->close();
	}

	if( polyline )
	{
		VShapeCmd* cmd = new VShapeCmd(
			&view()->part()->document(),
			i18n("Polyline"),
			polyline );

		view()->part()->addCommand( cmd, true );
		view()->selectionChanged();
	}
} // VPolylineTool::deactivate

void VPolylineTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	if(bezierPoints.count() > 2 )
	{
		VComposite polyline( 0L );
		polyline.moveTo( *bezierPoints.first() );
		KoPoint *p2, *p3, *p4;
		while( ( p2 = bezierPoints.next() ) && ( p3 = bezierPoints.next() ) && ( p4 = bezierPoints.next() ) )
		{
			polyline.curveTo( *p2, *p3, *p4 );
		}
		polyline.setState( VComposite::edit );
		polyline.draw( painter, &polyline.boundingBox() );
	}
} // VPolylineTool::draw

void VPolylineTool::drawBezierVector( KoPoint& start, KoPoint& end )
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
	painter->moveTo( KoPoint( end.x() - width / zoomFactor, end.y() - width / zoomFactor ) );
	painter->lineTo( KoPoint( end.x() + width / zoomFactor, end.y() - width / zoomFactor ) );
	painter->lineTo( KoPoint( end.x() + width / zoomFactor, end.y() + width / zoomFactor ) );
	painter->lineTo( KoPoint( end.x() - width / zoomFactor, end.y() + width / zoomFactor ) );
	painter->lineTo( KoPoint( end.x() - width / zoomFactor, end.y() - width / zoomFactor ) );
	painter->strokePath();
	painter->restore();
} // VPolylineTool::drawBezierVector

void VPolylineTool::mouseMove()
{
	if( bezierPoints.count() != 0 )
	{
		draw();

		bezierPoints.removeLast();
		bezierPoints.removeLast();
		bezierPoints.append( new KoPoint( last() ) );
		bezierPoints.append( new KoPoint( last() ) );

		draw();
	}
} // VPolylineTool::mouseMove

void VPolylineTool::mouseButtonPress()
{
	shiftPressed = ctrlPressed = false;
	if( bezierPoints.count() != 0 )
	{
		draw();
		bezierPoints.removeLast();
		bezierPoints.removeLast();
		bezierPoints.append( new KoPoint( last() ) );
	}

	lastVectorEnd = lastVectorStart = last();

	bezierPoints.append( new KoPoint( last() ) );
	bezierPoints.append( new KoPoint( last() ) );
	
	drawBezierVector( lastVectorStart, lastVectorEnd );
	draw();
} // VPolylineTool::mouseButtonPress()

void VPolylineTool::mouseButtonRelease()
{
	if( bezierPoints.count() == 2 )
	{
		drawBezierVector( lastVectorStart, lastVectorEnd );

		bezierPoints.removeLast();
		bezierPoints.append( new KoPoint(last() ) );    

		VPainter* painter = view()->painterFactory()->editpainter();
		painter->save();
		view()->canvasWidget()->setYMirroring( true );
		painter->setZoomFactor( view()->zoom() );
		painter->setRasterOp( Qt::XorROP );
		VStroke stroke( Qt::yellow, 0L, 1.0 );
		painter->setPen( stroke );
		painter->setBrush( Qt::yellow );
		painter->newPath();
		painter->drawNode( lastVectorStart, 2 );
		painter->strokePath();
		painter->restore();
	}
	else
	{
		drawBezierVector( lastVectorStart, lastVectorEnd );
		draw();

		bezierPoints.removeLast();
		KoPoint* p = bezierPoints.last();
		bezierPoints.removeLast();
		KoPoint* b = bezierPoints.last();
		bezierPoints.removeLast();

		if( shiftPressed )
		{
			bezierPoints.removeLast();
			bezierPoints.append( new KoPoint( *bezierPoints.last() ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p ) );
			lastVectorStart = lastVectorEnd = *p;
		}
		else if( ctrlPressed )
		{
			bezierPoints.removeLast();
			lastVectorStart = *bezierPoints.last();
			bezierPoints.append( new KoPoint( last() ) );
			bezierPoints.append( new KoPoint( *b ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			lastVectorEnd = last();
		}
		else
		{
			bezierPoints.append( new KoPoint( last() ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p - ( last() - *p ) ) );
			lastVectorStart = *p;
			lastVectorEnd = last();
		}
		if( bezierPoints.count() > 2 && p->isNear( *bezierPoints.first(), 3 ) )
		{
			bezierPoints.removeLast();
			m_close = true;
			accept();
			return;
		}
	}

	bezierPoints.append( new KoPoint( last() ) );
	bezierPoints.append( new KoPoint( last() ) );

	draw();
} // VPolylineTool::mouseButtonRelease

void VPolylineTool::mouseButtonDblClick()
{
	accept();
} // VPolylineTool::mouseButtonDlbClick()

void VPolylineTool::mouseDrag()
{
	if( bezierPoints.count() == 2 )
	{
		drawBezierVector( lastVectorStart, lastVectorEnd );

		bezierPoints.removeLast();
		bezierPoints.append( new KoPoint( last() ) );    
		lastVectorEnd = last();

		drawBezierVector( lastVectorStart, lastVectorEnd );
	}
	else
	{
		drawBezierVector( lastVectorStart, lastVectorEnd );
		draw();

		bezierPoints.removeLast();
		KoPoint* p = bezierPoints.last();
		bezierPoints.removeLast();
		KoPoint* b = bezierPoints.last();
		bezierPoints.removeLast();

		if( shiftPressed )
		{
			bezierPoints.removeLast();
			bezierPoints.append( new KoPoint( *bezierPoints.last() ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p ) );
			lastVectorStart = lastVectorEnd = *p;
		}
		else if( ctrlPressed )
		{
			bezierPoints.removeLast();
			lastVectorStart = *bezierPoints.last();
			bezierPoints.append( new KoPoint( last() ) );
			bezierPoints.append( new KoPoint( *b ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			lastVectorEnd = last();
		}
		else
		{
			bezierPoints.append( new KoPoint( last() ) );
			bezierPoints.append( new KoPoint( *p ) );
			bezierPoints.append( new KoPoint( *p - ( last() - *p ) ) );
			lastVectorStart = *p;
			lastVectorEnd = last();
		}

		draw();
		drawBezierVector( lastVectorStart, lastVectorEnd );
	}
} // VPolylineTool::mouseDrag()

void VPolylineTool::mouseDragRelease()
{
	mouseButtonRelease();
} // VPolylineTool::mouseDragRelease

void VPolylineTool::mouseDragShiftPressed()
{
	shiftPressed = true;
	// Emits a mouse move event. Sorry...
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, 1, 1 );
} // VPolylineTool::mouseDragShiftPressed

void VPolylineTool::mouseDragCtrlPressed()
{
		// Moves the mouse to the other bezier vector position.
	KoPoint p = *bezierPoints.at( bezierPoints.count() - 4) - *bezierPoints.at( bezierPoints.count() - 3 );
	int x = (int)( p.x() * view()->zoom() );
	int y = (int)( p.y() * view()->zoom() );
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );

	ctrlPressed = true;
} // VPolylineTool::mouseDragCtrlPressed

void VPolylineTool::mouseDragShiftReleased()
{
	shiftPressed = false;
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, -1, -1 );
} // VPolylineTool::mouseDragShiftReleased

void VPolylineTool::mouseDragCtrlReleased()
{
	KoPoint p = *bezierPoints.at( bezierPoints.count() - 3) - *bezierPoints.at( bezierPoints.count() - 4 );
	int x = (int)( p.x() * view()->zoom() );
	int y = (int)( p.y() * view()->zoom() );
	XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );

	ctrlPressed = false;
} // VPolylineTool::mouseDragCtrlReleased

void VPolylineTool::cancel()
{
	draw();
	
	bezierPoints.clear();
} // VPolylineTool::cancel

void VPolylineTool::cancelStep()
{
	draw();

	if ( bezierPoints.count() > 6 )
	{
		KoPoint p2 = *bezierPoints.last();
		bezierPoints.removeLast();
		bezierPoints.removeLast();
		bezierPoints.removeLast();
		KoPoint p1 = *bezierPoints.last();
		bezierPoints.removeLast();
		bezierPoints.removeLast();
		bezierPoints.append( new KoPoint( p1 ) );
		bezierPoints.append( new KoPoint( p1 ) );
		KoPoint p = p1 - p2;
		int x = (int)( p.x() * view()->zoom() );
		int y = (int)( p.y() * view()->zoom() );
		XWarpPointer( qt_xdisplay(), 0, 0, 0, 0, 0, 0, x, -y );
	}
	else
	{
		bezierPoints.clear();
	}

	draw();
} // VPolylineTool::cancelStep

void VPolylineTool::accept()
{
	deactivate();
	doActivate();
} // VPolylineTool::accept
