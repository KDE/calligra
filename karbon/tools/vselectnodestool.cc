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

#include <math.h>

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>
#include <koPoint.h>
#include <koRect.h>

#include <karbon_part.h>
#include <karbon_view.h>

#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <visitors/vselectnodes.h>
#include <commands/vtransformcmd.h>
#include <visitors/vdrawselection.h>
#include <core/vselection.h>
#include "vselectnodestool.h"
#include <vtransformnodes.h>
#include <commands/vdeletenodescmd.h>
#include <widgets/vcanvas.h>

#include <kdebug.h>

VSelectNodesTool::VSelectNodesTool( KarbonPart* part )
	: VTool( part, "selectnodes" ), m_state( normal )
{
	registerTool( this );
}

VSelectNodesTool::~VSelectNodesTool()
{
}

void
VSelectNodesTool::activate()
{
	view()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->showHandle( false );
	view()->part()->document().selection()->setSelectObjects( false );
}

QString
VSelectNodesTool::statusText()
{
	if( m_state == normal )
		return i18n( "Editing Nodes" );
	else
		return QString( "" );
}

void
VSelectNodesTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	double tolerance = 2.0 / view()->zoom();

	KoRect selrect( last().x() - tolerance, last().y() - tolerance,
					2 * tolerance + 1.0, 2 * tolerance + 1.0 );
	QPtrList<VSegment> segments = view()->part()->document().selection()->getSegments( selrect );
	if( view()->part()->document().selection()->objects().count() > 0 &&
		m_state != dragging && ( m_state >= moving || segments.count() > 0 ) )
	{
		if( m_state == normal )
		{
			if( segments.count() == 1 && !selrect.contains( segments.at( 0 )->knot() ) )
			{
				if( selrect.contains( segments.at( 0 )->point( 1 ) ) )
//					segments.at( 0 )->prev()->type() != VSegment::curve )
				{
					m_state = movingbezier1;
					segments.at( 0 )->selectPoint( 1, false );
				}
				else
				{
					m_state = movingbezier2;
					segments.at( 0 )->selectPoint( 0, false );
				}
				view()->part()->document().selection()->append( selrect.normalize(), false, true );
			}
			else
				m_state = moving;
			recalc();
		}

		VDrawSelection op( m_objects, painter, true );
		VObjectListIterator itr = m_objects;
        for( ; itr.current(); ++itr )
			op.visit( *( itr.current() ) );
	}
	else
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( first().x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), first().y() ) );
		painter->strokePath();

		m_state = dragging;
	}
}

void
VSelectNodesTool::setCursor() const
{
	if( m_state == moving ) return;

	double tolerance = 2.0 / view()->zoom();

	KoRect selrect( last().x() - tolerance, last().y() - tolerance,
					2 * tolerance + 1.0, 2 * tolerance + 1.0 );
	QPtrList<VSegment> segments = view()->part()->document().selection()->getSegments( selrect );
	if( segments.count() > 0 &&
		( segments.at( 0 )->knotIsSelected() ||
		  segments.at( 0 )->pointIsSelected( 0 ) ||
		  segments.at( 0 )->pointIsSelected( 1 ) ||
		  selrect.contains( segments.at( 0 )->knot() ) ) )
	{
		view()->setCursor( QCursor( Qt::CrossCursor ) );
	}
	else
		view()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VSelectNodesTool::mouseButtonPress()
{
	m_current = first();

	m_state = normal;

	recalc();

	view()->part()->document().selection()->setState( VObject::edit );
	view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	view()->part()->document().selection()->setState( VObject::selected );

	draw();
}

bool
VSelectNodesTool::keyReleased( Qt::Key key )
{
	if( key == Qt::Key_Delete )
	{
		if( view()->part()->document().selection()->objects().count() > 0 )
			view()->part()->addCommand( new VDeleteNodeCmd( &view()->part()->document() ), true );

		return true;
	}
	return false;
}

void
VSelectNodesTool::mouseButtonRelease()
{
	// erase old object:
	draw();

	double tolerance = 2.0 / view()->zoom();
	KoRect selrect( last().x() - tolerance, last().y() - tolerance,
					2 * tolerance + 1.0, 2 * tolerance + 1.0 );

	view()->part()->document().selection()->append();	// select all
	view()->part()->document().selection()->append( selrect.normalize(), false, true );

	view()->selectionChanged();
	view()->part()->repaintAllViews();
	m_state = normal;
}

void
VSelectNodesTool::mouseDrag()
{
	draw();

	recalc();

	draw();
}

void
VSelectNodesTool::mouseDragRelease()
{
	if( m_state >= moving )
	{
		view()->part()->document().selection()->setState( VObject::selected );
		VCommand *cmd;
		QPtrList<VSegment> segments;
		if( m_state == movingbezier1 || m_state == movingbezier2 )
		{
			double tolerance = 2.0 / view()->zoom();

			KoRect selrect( first().x() - tolerance, first().y() - tolerance,
							2 * tolerance + 1.0, 2 * tolerance + 1.0 );
			segments = view()->part()->document().selection()->getSegments( selrect );
			cmd = new VTranslateBezierCmd( segments.at( 0 ),
					qRound( ( last().x() - first().x() ) ),
					qRound( ( last().y() - first().y() ) ),
					m_state == movingbezier2 );
		}
		else
		{
			cmd = new VTranslatePointCmd(
					&view()->part()->document(),
					qRound( ( last().x() - first().x() ) ),
					qRound( ( last().y() - first().y() ) ) );
		}
		view()->part()->addCommand( cmd, true );
		m_state = normal;
	}
	else
	{
		KoPoint fp = first();
		KoPoint lp = last();

		if ( (fabs(lp.x() - fp.x()) + fabs(lp.y()-fp.y())) < 3.0 )
		{
			// AK - should take the middle point here
			fp = last() - KoPoint(8.0, 8.0);
			lp = last() + KoPoint(8.0, 8.0);
		}

		// erase old object:
		draw();

		view()->part()->document().selection()->append();	// select all
		view()->part()->document().selection()->append(
			KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize(),
			false );

		view()->selectionChanged();
		view()->part()->repaintAllViews();
		m_state = normal;
	}
}

void
VSelectNodesTool::cancel()
{
	// Erase old object:
	if ( isDragging() )
	{
		draw();
		m_state = normal;
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
}

void
VSelectNodesTool::recalc()
{
	if( m_state == dragging )
	{
		m_current = last();
	}
	else if( m_state == moving || m_state == movingbezier1 || m_state == movingbezier2 )
	{
		KoPoint _last = view()->canvasWidget()->snapToGrid( last() );
		double distx = _last.x() - first().x();
		double disty = _last.y() - first().y();
		// move operation
		QWMatrix mat;
		mat.translate( distx, disty );

		// Copy selected objects and transform:
		m_objects.clear();
		VObject* copy;

		VTransformNodes op( mat );

		VObjectListIterator itr = view()->part()->document().selection()->objects();
		for ( ; itr.current() ; ++itr )
		{
			if( itr.current()->state() != VObject::deleted )
			{
				copy = itr.current()->clone();
				copy->setState( VObject::edit );
				op.visit( *copy );
				m_objects.append( copy );
			}
		}
	}
}
