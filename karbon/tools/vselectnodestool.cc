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
#include "vselection.h"
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
VSelectNodesTool::activate()
{
	//if( m_state == normal )
		view()->statusMessage()->setText( i18n( "EditNode" ) );
	//else
//		view()->statusMessage()->setText( i18n( "Scale" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->setState( VObject::edit );
}

void
VSelectNodesTool::draw()
{
/*
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
	KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x() / view()->zoom(), m_lp.y() / view()->zoom() ) );

	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->objects().count() > 0 &&
		m_state != dragging &&
		( m_state == moving || view()->part()->document().selection()->pathNode(
			KoRect(
				lp.x() - tolerance,
				lp.y() - tolerance,
				2 * tolerance + 1.0,
				2 * tolerance + 1.0 ) ) ) )
	{
		if( m_state == normal )
		{
			double tolerance = 1.0 / view()->zoom();
			view()->part()->document().selection()->append(
				KoRect(
					fp.x() - tolerance,
					fp.y() - tolerance,
					2 * tolerance + 1.0,
					2 * tolerance + 1.0 ).normalize(),
				false );
			m_state = moving;
		}

		// move operation
		QWMatrix mat;
		mat.translate(	( m_lp.x() - fp.x() ) / view()->zoom(),
						( m_lp.y() - fp.y() ) / view()->zoom() );

		VTransformNodes op( mat );

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = view()->part()->document().selection()->objects();
		VObjectList list;
		list.setAutoDelete( true );
		for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}

		VObjectListIterator itr2 = list;
		painter->setZoomFactor( view()->zoom() );
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->setState( VObject::edit );
			op.visit( *itr2.current() );

			itr2.current()->draw(
				painter,
				itr2.current()->boundingBox() );
		}

		painter->setZoomFactor( 1.0 );
	}
	else
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->strokePath();

		m_state = dragging;
	}

	view()->painterFactory()->painter()->end();
*/
}

void
VSelectNodesTool::setCursor() const
{
/*
	if( !m_isDragging ) return;

	KoPoint lpp = KoPoint( p.x() / view()->zoom(), p.y() / view()->zoom() );
	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->pathNode(
		KoRect(
			lpp.x() - tolerance,
			lpp.y() - tolerance,
			2 * tolerance + 1,
			2 * tolerance * 1 ) ) )
	{
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::CrossCursor ) );
	}
	else
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
*/
}

void
VSelectNodesTool::mouseDragRelease()
{
/*
	if( m_state == moving )
	{
		m_state = normal;

		VTranslateCmd *cmd = new VTranslateCmd(
				&view()->part()->document(),
				qRound( ( lp.x() - fp.x() ) * ( 1.0 / view()->zoom() ) ),
				qRound( ( lp.y() - fp.y() ) * ( 1.0 / view()->zoom() ) ) );
		view()->part()->addCommand( cmd, true );

//			view()->part()->repaintAllViews();
	}
	else
	{
		fp.setX( fp.x() / view()->zoom() );
		fp.setY( fp.y() / view()->zoom() );
		lp.setX( lp.x() / view()->zoom() );
		lp.setY( lp.y() / view()->zoom() );

		if ( (fabs(lp.x()-fp.x()) + fabs(lp.y()-fp.y())) < 3.0 )
		{
			// AK - should take the middle point here
			fp = lp - KoPoint(8.0, 8.0);
			lp = lp + KoPoint(8.0, 8.0);
		}

		// erase old object:
		drawTemporaryObject();

		view()->part()->document().selection()->clear();
		view()->part()->document().selection()->append(
			KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize(),
			false );

		view()->selectionChanged();
		view()->part()->repaintAllViews();
		m_state = normal;
	}
*/
}
