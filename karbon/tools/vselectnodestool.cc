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

#include <math.h>

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>
#include <koPoint.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselectnodestool.h"
#include "vtransformcmd.h"
#include "vtransformnodes.h"

#include <kdebug.h>


VSelectNodesTool::VSelectNodesTool( KarbonView* view )
	: VTool( view ), m_state( normal )
{
}

VSelectNodesTool::~VSelectNodesTool()
{
}

void
VSelectNodesTool::doActivate()
{
	if( m_state == normal )
		view()->statusMessage()->setText( i18n( "EditNode" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->setState( VObject::edit );
}

void
VSelectNodesTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->objects().count() > 0 &&
		m_state != dragging &&
		( m_state == moving || view()->part()->document().selection()->pathNode(
			KoRect(
				last().x() - tolerance,
				last().y() - tolerance,
				2 * tolerance + 1.0,
				2 * tolerance + 1.0 ) ) ) )
	{
		if( m_state == normal )
		{
			double tolerance = 1.0 / view()->zoom();
			view()->part()->document().selection()->append(
				KoRect(
					first().x() - tolerance,
					first().y() - tolerance,
					2 * tolerance + 1.0,
					2 * tolerance + 1.0 ).normalize(),
				false );
			m_state = moving;
			recalc();
		}

		VObjectListIterator itr = m_objects;
        for( ; itr.current(); ++itr )
        {
			itr.current()->draw( painter, &itr.current()->boundingBox() );
		}
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

	//view()->painterFactory()->painter()->end();
}

void
VSelectNodesTool::setCursor() const
{
	//if( !m_isDragging ) return;

	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->pathNode(
		KoRect(
			last().x() - tolerance,
			last().y() - tolerance,
			2 * tolerance + 1,
			2 * tolerance * 1 ) ) )
	{
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::CrossCursor ) );
	}
	else
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VSelectNodesTool::mouseButtonPress()
{
	m_current = first();

	m_state = normal;

	recalc();

	draw();
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
	if( m_state == moving )
	{
		m_state = normal;

		VTranslateCmd *cmd = new VTranslateCmd(
				&view()->part()->document(),
				qRound( ( last().x() - first().x() ) * ( 1.0 / view()->zoom() ) ),
				qRound( ( last().y() - first().y() ) * ( 1.0 / view()->zoom() ) ) );
		view()->part()->addCommand( cmd, true );

//			view()->part()->repaintAllViews();
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

		view()->part()->document().selection()->clear();
		view()->part()->document().selection()->append(
			KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize(),
			false );

		view()->selectionChanged();
		view()->part()->repaintAllViews();
		m_state = normal;
	}
}

void
VSelectNodesTool::recalc()
{
	if( m_state == dragging )
	{
		m_current = last();
	}
	else if( m_state == moving )
	{
		// move operation
		QWMatrix mat;
		mat.translate(	( last().x() - first().x() ) / view()->zoom(),
						( last().y() - first().y() ) / view()->zoom() );

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
