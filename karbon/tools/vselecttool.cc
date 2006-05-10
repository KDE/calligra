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

#include <math.h>
#include <stdlib.h>

#include <qcursor.h>
#include <QLabel>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <KoPoint.h>
#include <KoRect.h>
#include <kdebug.h>

#include <karbon_part.h>
#include <karbon_view.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <core/vselection.h>
#include "vselecttool.h"
#include <commands/vtransformcmd.h>
#include <visitors/vselectiondesc.h>
#include <visitors/vselectobjects.h>
#include <widgets/vcanvas.h>

VSelectOptionsWidget::VSelectOptionsWidget( KarbonPart *part )
	: KDialogBase( 0L, "", true, i18n( "Selection" ), Ok | Cancel ), m_part( part )
{
	QButtonGroup *group = new QButtonGroup( 1, Qt::Horizontal, i18n( "Selection Mode" ), this );

	new QRadioButton( i18n( "Select in current layer" ), group );
	new QRadioButton( i18n( "Select in visible layers" ), group );
	new QRadioButton( i18n( "Select in selected layers" ), group );

	group->setRadioButtonExclusive( true );
	group->setButton( part->document().selectionMode() );

	connect( group, SIGNAL( clicked( int ) ), this, SLOT( modeChange( int ) ) );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	setFixedSize( baseSize() );
} // VSelectOptionsWidget::VSelectOptionsWidget

void VSelectOptionsWidget::modeChange( int mode )
{
	m_part->document().setSelectionMode( (VDocument::VSelectionMode)mode );
} // VSelectOptionsWidget::modeChanged

VSelectTool::VSelectTool( KarbonView *view )
	: VTool( view, "tool_select" ), m_state( normal )
{
	m_lock = false;
	m_add = true;
	m_objects.setAutoDelete( true );
	m_optionsWidget = new VSelectOptionsWidget( view->part() );
	registerTool( this );
	connect( view, SIGNAL( selectionChange() ), this, SLOT( updateStatusBar() ) );
}

VSelectTool::~VSelectTool()
{
	delete m_optionsWidget;
}

void
VSelectTool::activate()
{
	VTool::activate();
	view()->setCursor( QCursor( Qt::ArrowCursor ) );
	view()->part()->document().selection()->showHandle();
	view()->part()->document().selection()->setSelectObjects();
	view()->part()->document().selection()->setState( VObject::selected );
	view()->part()->document().selection()->selectNodes();
	view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	updateStatusBar();
}

QString
VSelectTool::statusText()
{
	return i18n( "Select" );
}

QString VSelectTool::contextHelp()
{
	QString s = i18n( "<qt><b>Selection tool:</b><br>" );
	s += i18n( "<i>Select in current layer:</i><br>The selection is made in the layer selected in the layers docker.<br><br>" );
	s += i18n( "<i>Select in visible layers:</i><br>The selection is made in the visible layers (eye in the layers docker).<br><br>" );
	s += i18n( "<i>Select in selected layers:</i><br>The selection is made in the checked layers in the layers docker.<br><br>" );
	s += i18n( "<i>Position using arrow keys</i><br>The selection can be positioned up, down, left and right using the corresponding arrow keys." );
	return s;
} // VSelectTool::contextHelp

void
VSelectTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	//painter->setZoomFactor( view()->zoom() );
	painter->setRasterOp( Qt::NotROP );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	if( m_state != normal )
	{
		VObjectListIterator itr = m_objects;
		for( ; itr.current(); ++itr )
		{
			itr.current()->draw( painter, &itr.current()->boundingBox() );
		}
	}
	else if( m_state == normal )
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( first().x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), first().y() ) );
		painter->strokePath();

		m_state = normal;
	}
}

void
VSelectTool::setCursor() const
{
	if( m_state != normal || !view() ) return;
	switch( view()->part()->document().selection()->handleNode( last() ) )
	{
		case node_lt:
		case node_rb:
			view()->setCursor( QCursor( Qt::SizeFDiagCursor ) );
			break;
		case node_rt:
		case node_lb:
			view()->setCursor( QCursor( Qt::SizeBDiagCursor ) );
			break;
		case node_lm:
		case node_rm:
			view()->setCursor( QCursor( Qt::SizeHorCursor ) );
			break;
		case node_mt:
		case node_mb:
			view()->setCursor( QCursor( Qt::SizeVerCursor ) );
			break;
		default:
			view()->setCursor( QCursor( Qt::ArrowCursor ) );
	}
}

void
VSelectTool::mouseButtonPress()
{
	// we are adding to the selection
	m_add = true;

	m_current = first();

	m_activeNode = view()->part()->document().selection()->handleNode( first() );
	KoRect rect = view()->part()->document().selection()->boundingBox();

	if( m_activeNode != node_none )
		m_state = scaling;
	else if( rect.contains( m_current ) && m_state == normal )
		m_state = moving;

	recalc();

	// undraw selection bounding box
	view()->part()->document().selection()->setState( VObject::edit );
	view()->repaintAll( rect );
	view()->part()->document().selection()->setState( VObject::selected );

	draw();
}

void
VSelectTool::rightMouseButtonPress()
{
	// we are removing from the selection
	m_add = false;

	m_current = first();

	recalc();

	// undraw selection bounding box
	view()->part()->document().selection()->setState( VObject::edit );
	view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	view()->part()->document().selection()->setState( VObject::selected );

	draw();
}

void
VSelectTool::mouseDrag()
{
	draw();

	recalc();

	draw();
}

void
VSelectTool::rightMouseButtonRelease()
{
	m_state = normal;
	m_add = true;

	if( ctrlPressed() )
	{
		// unselect the topmost object under the mouse cursor
		VObjectList newSelection;
		VSelectObjects selector( newSelection, first() );
		if( selector.visit( view()->part()->document() ) )
			view()->part()->document().selection()->take( *newSelection.last() );

		view()->part()->repaintAllViews( view()->part()->document().selection()->boundingBox() );
		view()->selectionChanged();

		updateStatusBar();
	}
	else if( view()->part()->document().selection()->objects().count() > 0 )
	{
		view()->showSelectionPopupMenu( QCursor::pos() );
	}
}

void
VSelectTool::mouseButtonRelease()
{
	m_state = normal;
	m_add = true;

	// selection of next underlying object
	if( shiftPressed() ) 
	{
		VObjectList newSelection;
		VObjectList oldSelection = view()->part()->document().selection()->objects();

		// clear selection if not in multi-slection-mode
		if( ! ctrlPressed() )
			view()->part()->document().selection()->clear();

		// get a list of all object under the mouse cursor
		VSelectObjects selector( newSelection, first(), true, true );
		if( selector.visit( view()->part()->document() ) )
		{
			// determine the last selected object of the object stack
			VObject *lastMatched = 0L;
			VObjectListIterator it( newSelection );
			for( ; it.current(); ++it )
			{
				if( oldSelection.contains( it.current() ) )
					lastMatched = it.current();
			}
			
			// select the next underlying object or the first if:
			// - none is selected
			// - the stack's bottom object was the last selected object
			if( lastMatched && lastMatched != newSelection.first() )
				view()->part()->document().selection()->append( newSelection.at( newSelection.find( lastMatched )-1 ) );
			else
				view()->part()->document().selection()->append( newSelection.last() );
		}
	}
	else	
	{
		// clear selection if not in multi-slection-mode
		if( ! ctrlPressed() )
			view()->part()->document().selection()->clear();

		// append the topmost object under the mouse cursor to the selection
		VObjectList newSelection;
		VSelectObjects selector( newSelection, first() );
		if( selector.visit( view()->part()->document() ) )
			view()->part()->document().selection()->append( newSelection.last() );
	}

	view()->part()->repaintAllViews( view()->part()->document().selection()->boundingBox() );
	view()->selectionChanged();

	updateStatusBar();
}

void
VSelectTool::mouseDragRelease()
{
	if( m_state == normal )
	{
		// Y mirroring
		KoPoint fp = first();
		KoPoint lp = last();
		if( ! ctrlPressed() )
			view()->part()->document().selection()->clear();
		
		KoRect selRect = KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize();
		if( m_add )
			view()->part()->document().selection()->append( selRect );
		else
			view()->part()->document().selection()->take( selRect );
		view()->part()->repaintAllViews( selRect );
	}
	else if( m_state == moving )
	{
		m_state = normal;
		recalc();
		if( m_lock )
			view()->part()->addCommand(
				 new VTranslateCmd(
					&view()->part()->document(),
					abs( int( m_distx ) ) >= abs( int( m_disty ) ) ? qRound( m_distx ) : 0,
					abs( int( m_distx ) ) <= abs( int( m_disty ) ) ? qRound( m_disty ) : 0, altPressed() ),
				true );
		else
			view()->part()->addCommand(
				new VTranslateCmd( &view()->part()->document(), qRound( m_distx ), qRound( m_disty ), altPressed() ),
				true );
	}
	else if( m_state == scaling )
	{
		m_state = normal;
		view()->part()->addCommand(
			new VScaleCmd( &view()->part()->document(), m_sp, m_s1, m_s2, altPressed() ),
			true );
		m_s1 = m_s2 = 1;
	}

	view()->selectionChanged();
	m_lock = false;
	updateStatusBar();
}

void
VSelectTool::arrowKeyReleased( Qt::Key key )
{
	int dx = 0;
	int dy = 0;
	switch( key )
	{
		case Qt::Key_Up: dy = 10; break;
		case Qt::Key_Down: dy = -10; break;
		case Qt::Key_Right: dx = 10; break;
		case Qt::Key_Left: dx = -10; break;
		default: return;
	}
	m_state = normal;
	view()->part()->addCommand(
		new VTranslateCmd(
			&view()->part()->document(),
			dx, dy ),
		true );
	view()->selectionChanged();
	updateStatusBar();
}

bool
VSelectTool::keyReleased( Qt::Key key )
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
		default: return false;
	}

	if( view() )
		view()->repaintAll( selection->boundingBox() );

	return true;
}

void
VSelectTool::updateStatusBar() const
{
	if( ! view() ) 
		return;

	if( ! view()->part() )
		return;

	int objcount = view()->part()->document().selection()->objects().count();
	if( objcount > 0 )
	{
		KoRect rect = view()->part()->document().selection()->boundingBox();

		double x = KoUnit::toUserValue( rect.x(), view()->part()->unit() );
		double y = KoUnit::toUserValue( rect.y(), view()->part()->unit() );
		double r = KoUnit::toUserValue( rect.right(), view()->part()->unit() );
		double b = KoUnit::toUserValue( rect.bottom(), view()->part()->unit() );

		// print bottom-left (%1,%2), top-right (%3,%4) corner of selection bounding box and document unit (%5)
		QString selectMessage = i18n( "[(left,bottom), (right,top)] (actual unit)", "Selection [(%1, %2), (%3, %4)] (%5)").arg( x, 0, 'f', 1 ).arg( y, 0, 'f', 1 ).arg( r, 0, 'f', 1 ).arg( b, 0, 'f', 1 ).arg( view()->part()->unitName() );

		VSelectionDescription selectionDesc;
		selectionDesc.visit( *view()->part()->document().selection() );
		selectMessage += QString( "(%1)" ).arg( selectionDesc.description() );

		view()->statusMessage()->setText( selectMessage );
	}
	else
		view()->statusMessage()->setText( i18n( "No selection" ) );
}

void
VSelectTool::mouseDragCtrlPressed()
{
	m_lock = true;
}

void
VSelectTool::mouseDragCtrlReleased()
{
	m_lock = false;
}

void
VSelectTool::mouseDragShiftPressed()
{
	draw();

	recalc();

	draw();
}

void
VSelectTool::mouseDragShiftReleased()
{
	draw();

	recalc();

	draw();
}

void
VSelectTool::cancel()
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
VSelectTool::recalc()
{
	if( m_state == normal )
	{
		m_current = last();
	}
	else
	{
		VTransformCmd* cmd;
		KoPoint _first = view()->canvasWidget()->snapToGrid( first() );
		KoPoint _last = view()->canvasWidget()->snapToGrid( last() );
		KoRect rect = view()->part()->document().selection()->boundingBox();

		if( m_state == moving )
		{
			KoPoint p( rect.x() + last().x() - first().x(), rect.bottom() + last().y() - first().y() );
			p = view()->canvasWidget()->snapToGrid( p );
			m_distx = p.x() - rect.x();
			m_disty = p.y() - rect.bottom();
			if( m_lock )
				cmd = new VTranslateCmd( 0L, abs( int( m_distx ) ) >= abs( int( m_disty ) ) ? m_distx : 0,
											 abs( int( m_distx ) ) <= abs( int( m_disty ) ) ? m_disty : 0 );
			else
				cmd = new VTranslateCmd( 0L, m_distx, m_disty );
		}
		else
		{
			if( m_activeNode == node_lb )
			{
				m_sp = KoPoint( rect.right(), rect.bottom() );
				m_s1 = ( rect.right() - _last.x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - _last.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mb )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.bottom() );
				m_s1 = 1;
				m_s2 = ( rect.bottom() - _last.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rb )
			{
				m_sp = KoPoint( rect.x(), rect.bottom() );
				m_s1 = ( _last.x() - rect.x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - _last.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rm)
			{
				m_sp = KoPoint( rect.x(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( _last.x() - rect.x() ) / double( rect.width() );
				m_s2 = 1;
			}
			else if( m_activeNode == node_rt )
			{
				m_sp = KoPoint( rect.x(), rect.y() );
				m_s1 = ( _last.x() - rect.x() ) / double( rect.width() );
				m_s2 = ( _last.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mt )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.y() );
				m_s1 = 1;
				m_s2 = ( _last.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lt )
			{
				m_sp = KoPoint( rect.right(), rect.y() );
				m_s1 = ( rect.right() - _last.x() ) / double( rect.width() );
				m_s2 = ( _last.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lm )
			{
				m_sp = KoPoint( rect.right(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( rect.right() - _last.x() ) / double( rect.width() );
				m_s2 = 1;
			}

			if( shiftPressed() )
				m_s1 = m_s2 = qMax( m_s1, m_s2 );
			cmd = new VScaleCmd( 0L, m_sp, m_s1, m_s2 );
		}

		// Copy selected objects and transform:
		m_objects.clear();
		VObject* copy;

		VObjectListIterator itr = view()->part()->document().selection()->objects();
		for( ; itr.current() ; ++itr )
		{
			if( itr.current()->state() != VObject::deleted )
			{
				copy = itr.current()->clone();
				copy->setState( VObject::edit );

				cmd->visit( *copy );

				m_objects.append( copy );
			}
		}

		delete( cmd );
	}
}

bool
VSelectTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void 
VSelectTool::refreshUnit()
{
	updateStatusBar();
}

void
VSelectTool::setup( KActionCollection *collection )
{
       m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KRadioAction( i18n( "Select Tool" ), "14_select", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Select" ) );
		m_action->setExclusiveGroup( "select" );
		//m_ownAction = true;
	}
}

#include "vselecttool.moc"
