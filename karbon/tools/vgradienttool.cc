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
#include <qlabel.h>

#include <klocale.h>

#include <karbon_part.h>
#include <karbon_view.h>
#include <karbon_factory.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include "vgradienttool.h"
#include <widgets/vgradienttabwidget.h>
#include <commands/vfillcmd.h>
#include <commands/vstrokecmd.h>
#include <core/vstroke.h>
#include <core/vselection.h>

#include <kdebug.h>

VGradientTool::VGradientOptionsWidget::VGradientOptionsWidget( VGradient *gradient )
	: KDialogBase( 0L, "", true, i18n( "Edit Gradient" ), Ok | Cancel )
{
	m_gradientWidget = new VGradientTabWidget( *gradient, KarbonFactory::rServer(), this );
	setMainWidget( m_gradientWidget );
	setFixedSize( baseSize() );
}

VGradientTool::VGradientTool( KarbonView *view )
	: VTool( view, "gradienttool" ), m_state( normal )
{
	setName( "tool_gradient" );
	m_optionsWidget = new VGradientOptionsWidget( &m_gradient );
	registerTool( this );
}

VGradientTool::~VGradientTool()
{
	delete m_optionsWidget;
}

void
VGradientTool::activate()
{
	view()->statusMessage()->setText( i18n( "Gradient" ) );
	view()->setCursor( QCursor( Qt::crossCursor ) );
	VTool::activate();
	draw( view()->painterFactory()->editpainter() );
}

QString
VGradientTool::statusText()
{
	return i18n( "Gradient tool" );
}

QString 
VGradientTool::contextHelp()
{
	QString s = i18n( "<qt><b>Gradient tool:</b><br>" );
	s += i18n( "<i>Click and drag</i> to choose the gradient vector.<br>" );
	s += i18n( "<br><b>Gradient editing:</b><br>" );
	s += i18n( "<i>Click and drag</i> to move points.<br>" );
	s += i18n( "<i>Double click</i> on a color point to edit it.<br>" );
	s += i18n( "<i>Right click</i> on a color point to remove it.</qt>" );
	return s;
}

void 
VGradientTool::draw( VPainter* painter )
{
	VSelection* selection = view()->part()->document().selection();
	if( selection->objects().count() != 1 ) return;
	
	VObject *obj = selection->objects().getFirst();
	// TODO find a way to determine if the fill or stroke is to be edited
	if( obj->fill()->type() == VFill::grad )
		m_gradient = obj->fill()->gradient();
	else
		return;

	KoPoint s = m_gradient.origin();
	KoPoint e = m_gradient.vector();

	double handleSize = 3.0;
	double zoom = view()->zoom();

	// save the handle rects for later inside testing
	m_start = KoRect( s.x()-handleSize, s.y()-handleSize, 2*handleSize, 2*handleSize );
	m_end = KoRect( e.x()-handleSize, e.y()-handleSize, 2*handleSize, 2*handleSize );

	// draw the gradient vector
	painter->newPath();
	painter->moveTo( s );
	painter->lineTo( e );
	painter->strokePath();

	// draw the handle rects
	painter->drawRect( KoRect( m_start.x()*zoom, m_start.y()*zoom, m_start.width(), m_start.height() ) );
	painter->drawRect( KoRect( m_end.x()*zoom, m_end.y()*zoom, m_end.width(), m_end.height() ) );
}

void
VGradientTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	
	// differentiate between moving a handle and creating a complete new vector
	if( m_state == moveStart || m_state == moveEnd )
	{
		painter->moveTo( m_fixed );
		painter->lineTo( m_current );
	}
	else if( m_state == createNew )
	{
		painter->moveTo( first() );
		painter->lineTo( m_current );
	}

	painter->strokePath();
}

void
VGradientTool::mouseDrag()
{
	if( m_state == normal ) 
		return;

	// undo old line
	draw();

	m_current = last();

	draw();
}

void
VGradientTool::mouseButtonPress()
{
	m_current = first();
	// set the apropriate editing state
	if( m_start.contains( m_current ) )
	{
		m_state = moveStart;
		m_fixed = m_end.center();
	}
	else if( m_end.contains( m_current ) )
	{
		m_state = moveEnd;
		m_fixed = m_start.center();
	}
	else 
		m_state = createNew;
}

void
VGradientTool::mouseButtonRelease()
{
	if( ! view() || view()->part()->document().selection()->objects().count() == 0 ) 
		return;

	// save old gradient position
	VGradient oldGradient = m_gradient;

	if( first() == last() )
	{
		if( showDialog() != QDialog::Accepted )
			return;
	}

	// use the old gradient position
	m_gradient.setVector( oldGradient.vector() );
	m_gradient.setOrigin( oldGradient.origin() );
	m_gradient.setFocalPoint( oldGradient.focalPoint() );

	/*
	m_gradient.setOrigin( first() );
	KoPoint p = last();
	if( first().x() == last().x() && first().y() == last().y() ) // workaround for a libart 2.3.10 bug
		p.setX( first().x() + 1 );
	m_gradient.setVector( p );
	*/

	if( m_optionsWidget->gradientWidget()->target() == VGradientTabWidget::FILL )
	{
		VFill fill;
		fill.gradient() = m_gradient;
		fill.setType( VFill::grad );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_gradient" ), true );
	}
	else
	{
		VStroke stroke;
		stroke.gradient() = m_gradient;
		stroke.setType( VStroke::grad );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke, "14_gradient" ), true );
	}
}

void
VGradientTool::mouseDragRelease()
{
	if( ! view() || m_state == normal ) 
		return;

	if( view()->part()->document().selection()->objects().count() == 0 )
	{
		draw();
		return;
	}

	if( m_state == moveStart )
	{
		m_gradient.setOrigin( last() );
		m_gradient.setFocalPoint( last() );
		m_gradient.setVector( m_fixed );
	}
	else if( m_state == moveEnd )
	{
		m_gradient.setOrigin( m_fixed );
		m_gradient.setFocalPoint( m_fixed );
		m_gradient.setVector( last() );
	}
	else if( m_state == createNew )
	{
		m_gradient.setOrigin( first() );
		m_gradient.setFocalPoint( first() );
		m_gradient.setVector( last() );
	}

	if( m_optionsWidget->gradientWidget()->target() == VGradientTabWidget::FILL )
	{
		VFill fill;
		fill.gradient() = m_gradient;
		fill.setType( VFill::grad );
		VColor c = fill.color();
		c.setOpacity( m_optionsWidget->gradientWidget()->opacity() );
		fill.setColor( c, false );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill, "14_gradient" ), true );
	}
	else
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &m_gradient ), true );
}

void
VGradientTool::cancel()
{
	// Erase old object:
	if( isDragging() )
		draw();
}

bool
VGradientTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VGradientTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KRadioAction( i18n( "Gradient Tool" ), "14_gradient", Qt::Key_G, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Gradient" ) );
		m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

void
VGradientTool::setCursor() const
{
	if( !view() ) return;

	// set a different cursor if mouse is inside the handle rects
	if( m_start.contains( last() ) || m_end.contains( last() ) )
		view()->setCursor( QCursor( Qt::SizeAllCursor ) );
	else
		view()->setCursor( QCursor( Qt::arrowCursor ) );
}
