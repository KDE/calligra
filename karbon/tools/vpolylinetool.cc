/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
#include <qlabel.h>

#include <klocale.h>

#include <karbon_part.h>
#include <karbon_view.h>
#include <core/vcolor.h>
#include <core/vcomposite.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <core/vglobal.h>
#include <core/vcursor.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include "vpolylinetool.h"
#include <commands/vshapecmd.h>
#include <commands/vcommand.h>
#include <widgets/vcanvas.h>

VPolylineTool::VPolylineTool( KarbonView *view )
	: VTool( view, "tool_polyline" )
{
	m_bezierPoints.setAutoDelete( true );
	registerTool( this );
	m_crossCursor = new QCursor( VCursor::createCursor( VCursor::CrossHair ) );
}

VPolylineTool::~VPolylineTool()
{
	delete m_crossCursor;
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
	VTool::activate();
	view()->statusMessage()->setText( i18n( "Polyline Tool" ) );
	view()->setCursor( *m_crossCursor );

	m_bezierPoints.clear();
	m_close = false;

	connect( view()->part()->commandHistory(), SIGNAL(commandExecuted()), this, SLOT(commandExecuted()) );
}

void
VPolylineTool::initializePath( VPath &path )
{
	KoPoint* p1 = m_bezierPoints.first();
	KoPoint* p2;
	KoPoint* p3;
	KoPoint* p4;

	path.moveTo( *p1 );

	while(
		( p2 = m_bezierPoints.next() ) &&
		( p3 = m_bezierPoints.next() ) &&
		( p4 = m_bezierPoints.next() ) )
	{
		if ( *p1 == *p2 )
			if ( *p3 == *p4 )
				path.lineTo( *p4 );
			else
				//polyline->curve1To( *p3, *p4 );
				path.curveTo( *p3, *p4, *p4 );
		else
			if ( *p3 == *p4 )
				//polyline->curve2To( *p2, *p4 );
				path.curveTo( *p2, *p2, *p4 );
			else
				path.curveTo( *p2, *p3, *p4 );
		p1 = p4;
	}
}

void
VPolylineTool::createObject()
{
	VPath* polyline = 0L;
	if( m_bezierPoints.count() > 2 )
	{
		polyline = new VPath( 0L );
		if( polyline )
		{
			initializePath( *polyline );
			if( m_close )
				polyline->close();

			VShapeCmd* cmd = new VShapeCmd(
				&view()->part()->document(),
				i18n( "Polyline" ),
				polyline,
				"14_polyline" );
	
			view()->part()->addCommand( cmd, true );
		}
	}

	m_bezierPoints.clear();
	m_close = false;
}

void
VPolylineTool::deactivate()
{
	m_bezierPoints.removeLast();
	m_bezierPoints.removeLast();
	
	createObject();

	disconnect( view()->part()->commandHistory(), SIGNAL(commandExecuted()), this, SLOT(commandExecuted()) );
}

void
VPolylineTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	if( m_bezierPoints.count() > 2 )
	{
		VPath polyline( 0L );
		initializePath( polyline );

		polyline.setState( VObject::edit );
		polyline.draw( painter, &polyline.boundingBox() );
	}
}

void
VPolylineTool::drawBezierVector( KoPoint& start, KoPoint& end )
{
	VPainter* painter = view()->painterFactory()->editpainter();

	painter->save();

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
		KoPoint _last = view()->canvasWidget()->snapToGrid( last() );
		draw();

		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( _last ) );
		m_bezierPoints.append( new KoPoint( _last ) );

		draw();
	}
}

void
VPolylineTool::mouseButtonPress()
{
	KoPoint _last = view()->canvasWidget()->snapToGrid( last() );
	if( m_bezierPoints.count() != 0 )
	{
		draw();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( _last ) );
	}

	m_lastVectorEnd = m_lastVectorStart = _last;

	m_bezierPoints.append( new KoPoint( _last ) );
	m_bezierPoints.append( new KoPoint( _last ) );
	drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
	draw();
}

void
VPolylineTool::mouseButtonRelease()
{
	KoPoint _last = view()->canvasWidget()->snapToGrid( last() );
	if( m_bezierPoints.count() == 2 )
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );

		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( _last ) );

		VPainter* painter = view()->painterFactory()->editpainter();
		painter->save();
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
		KoPoint* p = new KoPoint( *m_bezierPoints.last() );
		m_bezierPoints.removeLast();
		KoPoint* b = new KoPoint( *m_bezierPoints.last() );
		m_bezierPoints.removeLast();

		if( shiftPressed() )
		{
			m_bezierPoints.removeLast();
			m_bezierPoints.append( new KoPoint( *m_bezierPoints.last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_lastVectorStart = m_lastVectorEnd = *p;
		}
		else if( ctrlPressed() )
		{
			m_bezierPoints.removeLast();
			m_lastVectorStart = *m_bezierPoints.last();
			m_bezierPoints.append( new KoPoint( _last ) );
			m_bezierPoints.append( new KoPoint( *b ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			m_lastVectorEnd = _last;
		}
		else
		{
			m_bezierPoints.append( new KoPoint( _last ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( _last - *p ) ) );
			m_lastVectorStart = *p;
			m_lastVectorEnd = _last;
		}
		if( m_bezierPoints.count() > 2 && p->isNear( *m_bezierPoints.first(), 3 ) )
		{
			m_bezierPoints.append( new KoPoint( _last ) );
			m_close = true;
			createObject();
			return;
		}
	}

	m_bezierPoints.append( new KoPoint( _last ) );
	m_bezierPoints.append( new KoPoint( _last ) );

	draw();
}

void
VPolylineTool::rightMouseButtonRelease()
{
	// end line without adding new points
	m_bezierPoints.removeLast();
	m_bezierPoints.removeLast();

	createObject();
}

void
VPolylineTool::mouseButtonDblClick()
{
	createObject();
}

void
VPolylineTool::mouseDrag()
{
	KoPoint _last = view()->canvasWidget()->snapToGrid( last() );

	if( m_bezierPoints.count() == 2 )
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );

		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( _last ) );
		m_lastVectorEnd = _last;

		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
	}
	else
	{
		drawBezierVector( m_lastVectorStart, m_lastVectorEnd );
		draw();

		m_bezierPoints.removeLast();
		KoPoint* p = new KoPoint( *m_bezierPoints.last() );
		m_bezierPoints.removeLast();
		KoPoint* b = new KoPoint( *m_bezierPoints.last() );
		m_bezierPoints.removeLast();

		if( shiftPressed() )
		{
			m_bezierPoints.removeLast();
			m_bezierPoints.append( new KoPoint( *m_bezierPoints.last() ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_lastVectorStart = m_lastVectorEnd = *p;
		}
		else if( ctrlPressed() )
		{
			m_bezierPoints.removeLast();
			m_lastVectorStart = *m_bezierPoints.last();
			m_bezierPoints.append( new KoPoint( _last ) );
			m_bezierPoints.append( new KoPoint( *b ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( *b - *p ) ) );
			m_lastVectorEnd = _last;
		}
		else
		{
			m_bezierPoints.append( new KoPoint( _last ) );
			m_bezierPoints.append( new KoPoint( *p ) );
			m_bezierPoints.append( new KoPoint( *p - ( _last - *p ) ) );
			m_lastVectorStart = *p;
			m_lastVectorEnd = _last;
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
}

void
VPolylineTool::mouseDragCtrlPressed()
{
	// Moves the mouse to the other bezier vector position.
	if( m_bezierPoints.count() > 3 )
	{
		KoPoint p;
		p = *m_bezierPoints.at( m_bezierPoints.count() - 4) - *m_bezierPoints.at( m_bezierPoints.count() - 3 );

		view()->setPos( p );
	}
}

void
VPolylineTool::mouseDragShiftReleased()
{
}

void
VPolylineTool::mouseDragCtrlReleased()
{
	if( m_bezierPoints.count() > 3 )
	{
		KoPoint p;
		p = *m_bezierPoints.at( m_bezierPoints.count() - 3) - *m_bezierPoints.at( m_bezierPoints.count() - 4 );

		view()->setPos( p );
	}
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
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		KoPoint p1 = *m_bezierPoints.last();
		m_bezierPoints.removeLast();
		m_bezierPoints.removeLast();
		m_bezierPoints.append( new KoPoint( p1 ) );
		m_bezierPoints.append( new KoPoint( p1 ) );

		view()->setPos( p1 );
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
	activate();
}

void
VPolylineTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
	        KShortcut shortcut( Qt::Key_Plus );
      		shortcut.append( KShortcut( Qt::Key_F9 ) );
		m_action = new KRadioAction( i18n( "Polyline Tool" ), "14_polyline", shortcut, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Polyline" ) );
		m_action->setExclusiveGroup( "freehand" );
		//m_ownAction = true;
	}
}

void 
VPolylineTool::commandExecuted()
{
	cancel();
}

#include "vpolylinetool.moc"
