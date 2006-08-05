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
#include <QLabel>
//Added by qt3to4:
#include <Q3PtrList>

#include <klocale.h>
#include <QPointF>
#include <QRectF>
#include <kicon.h>

#include <karbon_part.h>
#include <karbon_view.h>

#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <visitors/vselectnodes.h>
#include <commands/vtransformcmd.h>
#include <visitors/vdrawselection.h>
#include <core/vselection.h>
#include <core/vcursor.h>
#include "vselectnodestool.h"
#include <vtransformnodes.h>
#include <commands/vdeletenodescmd.h>
#include <widgets/vcanvas.h>

#include <kactioncollection.h>

#include <kdebug.h>

VSelectNodesTool::VSelectNodesTool( KarbonView* view )
	: VTool( view, "tool_select_nodes" ), m_state( normal ), m_select( true )
{
	registerTool( this );
}

VSelectNodesTool::~VSelectNodesTool()
{
}

void
VSelectNodesTool::activate()
{
	if( view() )
	{
		view()->setCursor( VCursor::needleArrow() );
		view()->part()->document().selection()->showHandle( false );
		view()->part()->document().selection()->setSelectObjects( false );
		// deselect all nodes
		view()->part()->document().selection()->selectNodes( false );
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
	VTool::activate();
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
	// TODO: port to Qt4
	// painter->setRasterOp( Qt::NotROP );

	VSelection* selection = view()->part()->document().selection();

	QRectF selrect = calcSelRect( last() );	

	Q3PtrList<VSegment> segments = selection->getSegments( selrect );
	if( selection->objects().count() > 0 &&
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
				selection->append( selrect.normalized(), false, true );
			}
			else
			{
				m_state = moving;
				selection->append( selrect.normalized(), false, false );
			}

			recalc();
		}

		VDrawSelection op( m_objects, painter, true, selection->handleSize() );
		VObjectListIterator itr = m_objects;
        for( ; itr.current(); ++itr )
			op.visit( *( itr.current() ) );
	}
	else
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( QPointF( first().x(), first().y() ) );
		painter->lineTo( QPointF( m_current.x(), first().y() ) );
		painter->lineTo( QPointF( m_current.x(), m_current.y() ) );
		painter->lineTo( QPointF( first().x(), m_current.y() ) );
		painter->lineTo( QPointF( first().x(), first().y() ) );
		painter->strokePath();

		m_state = dragging;
	}
}

void
VSelectNodesTool::setCursor() const
{
	if( m_state == moving ) return;

	QRectF selrect = calcSelRect( last() );

	Q3PtrList<VSegment> segments = view()->part()->document().selection()->getSegments( selrect );
	if( segments.count() > 0 &&
		( segments.at( 0 )->knotIsSelected() ||
		  segments.at( 0 )->pointIsSelected( 0 ) ||
		  segments.at( 0 )->pointIsSelected( 1 ) ||
		  selrect.contains( segments.at( 0 )->knot() ) ) )
	{
		view()->setCursor( VCursor::needleMoveArrow() );
	}
	else
		view()->setCursor( VCursor::needleArrow() );
}

void
VSelectNodesTool::mouseButtonPress()
{
	m_current = first();

	m_state = normal;
	m_select = true;

	recalc();

	view()->part()->document().selection()->setState( VObject::edit );
	view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	view()->part()->document().selection()->setState( VObject::selected );

	draw();
}

void 
VSelectNodesTool::rightMouseButtonPress()
{
	m_current = first();

	m_state = normal;
	m_select = false;

	recalc();

	view()->part()->document().selection()->setState( VObject::edit );
	view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	view()->part()->document().selection()->setState( VObject::selected );

	draw();
}

bool
VSelectNodesTool::keyReleased( Qt::Key key )
{
	
	VSelection* selection = view()->part()->document().selection();

	switch( key )
	{
		// increase/decrease the handle size
		case Qt::Key_I:
		{
			uint handleSize = selection->handleSize();
			if( shiftPressed() ) 
				selection->setHandleSize( ++handleSize );
			else if( handleSize > 1 )
				selection->setHandleSize( --handleSize );
		}
		break;
		case Qt::Key_Delete:
			if( selection->objects().count() > 0 )
				view()->part()->addCommand( new VDeleteNodeCmd( &view()->part()->document() ), true );
		break;
		default: return false;
	}

	if( view() )
		view()->repaintAll( selection->boundingBox() );

	return true;
}

void
VSelectNodesTool::mouseButtonRelease()
{
	// erase old object:
	draw();

	VSelection* selection = view()->part()->document().selection();

	QRectF selrect = calcSelRect( last() );

	if( ctrlPressed() )
		selection->append( selrect.normalized(), false, false );
	else
		selection->append( selrect.normalized(), false, true );

	view()->selectionChanged();
	view()->part()->repaintAllViews();
	m_state = normal;
}

void 
VSelectNodesTool::rightMouseButtonRelease()
{
	// erase old object:
	draw();

	VSelection* selection = view()->part()->document().selection();

	QRectF selrect = calcSelRect( last() );

	selection->take( selrect.normalized(), false, false );

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
		Q3PtrList<VSegment> segments;
		if( m_state == movingbezier1 || m_state == movingbezier2 )
		{
			QRectF selrect = calcSelRect( first() );
			segments = view()->part()->document().selection()->getSegments( selrect );
			cmd = new VTranslateBezierCmd( &view()->part()->document(), segments.at( 0 ),
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
		QPointF fp = first();
		QPointF lp = last();

		if ( (fabs(lp.x() - fp.x()) + fabs(lp.y()-fp.y())) < 3.0 )
		{
			// AK - should take the middle point here
			fp = last() - QPointF(8.0, 8.0);
			lp = last() + QPointF(8.0, 8.0);
		}

		// erase old object:
		draw();

		if( m_select )
		{
			view()->part()->document().selection()->append();	// select all
			view()->part()->document().selection()->append(
				QRectF( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalized(),
				false );
		}
		else
		{
			view()->part()->document().selection()->take(
				QRectF( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalized(),
				false, false );
		}
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
		QPointF _last = QPointF(); // TODO: port: view()->canvasWidget()->snapToGrid( last() );
		double distx = _last.x() - first().x();
		double disty = _last.y() - first().y();
		// move operation
		QMatrix mat;
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

void
VSelectNodesTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_selectnodes" ), i18n( "Select Nodes Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::SHIFT+Qt::Key_H );
		m_action->setToolTip( i18n( "Select Nodes" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO port: m_action->setExclusiveGroup( "select" );
		//m_ownAction = true;
	}
}

QRectF 
VSelectNodesTool::calcSelRect( const QPointF &pos ) const
{
	double tolerance = view()->part()->document().selection()->handleSize() / view()->zoom();
	return QRectF( pos.x() - tolerance, pos.y() - tolerance, 2 * tolerance + 1.0, 2 * tolerance + 1.0 );
}
