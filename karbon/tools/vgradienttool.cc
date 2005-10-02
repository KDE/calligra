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

VGradientTool::VGradientTool( KarbonPart *part )
	: VTool( part, "gradienttool" )
{
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
VGradientTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	painter->setPen( Qt::DotLine );
	painter->newPath();
	painter->moveTo( first() );
	painter->lineTo( m_current );
	painter->strokePath();
}

void
VGradientTool::mouseDrag()
{
	// undo old line
	draw();

	m_current = last();

	draw();
}

void
VGradientTool::mouseButtonPress()
{
	m_current = first();
}

void
VGradientTool::mouseButtonRelease()
{
	if( view()->part()->document().selection()->objects().count() == 0 ) return;
	m_gradient.setOrigin( first() );
	KoPoint p = last();
	if( first().x() == last().x() && first().y() == last().y() ) // workaround for a libart 2.3.10 bug
		p.setX( first().x() + 1 );
	m_gradient.setVector( p );

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
	if( view()->part()->document().selection()->objects().count() == 0 )
	{
		draw();
		return;
	}
	// Y mirroring
	KoPoint fp = first();
	//fp.setY( -fp.y() + view()->canvasWidget()->viewport()->height() );
	KoPoint lp = last();
	//lp.setY( -lp.y() + view()->canvasWidget()->viewport()->height() );
	m_gradient.setOrigin( fp );
	m_gradient.setFocalPoint( fp );
	m_gradient.setVector( lp );

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

