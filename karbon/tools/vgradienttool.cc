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
#include <qlabel.h>

#include <klocale.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vgradienttool.h"
#include "vgradientdlg.h"
#include "vfillcmd.h"
#include "vstrokecmd.h"

#include <kdebug.h>
VGradientTool::VGradientTool( KarbonView* view )
	: VTool( view )
{
	m_dialog = new VGradientDlg();
	m_dialog->setGradientRepeat( VGradient::none );
	m_dialog->setGradientType( VGradient::linear );
	m_dialog->setGradientFill( 1 );
	m_dialog->setStartColor( Qt::red );
	m_dialog->setEndColor( Qt::yellow );
}

VGradientTool::~VGradientTool()
{
	delete m_dialog;
}

void
VGradientTool::activate()
{
	view()->statusMessage()->setText( i18n( "Gradient" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
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
VGradientTool::mouseDrag( const KoPoint& current )
{
	// undo old line
	draw();

	m_current = last();

	draw();
}

void
VGradientTool::mouseButtonPress( const KoPoint& current )
{
	m_current = first();
}

void
VGradientTool::mouseButtonRelease( const KoPoint& current )
{
	VGradient gradient;
	gradient.clearStops();
	gradient.addStop( VColor( m_dialog->startColor().rgb() ), 0.0, 0.5 );
	gradient.addStop( VColor( m_dialog->endColor().rgb() ), 1.0, 0.5 );
	gradient.setOrigin( first() );
	gradient.setVector( last() );
	gradient.setType( (VGradient::VGradientType)m_dialog->gradientType() );
	gradient.setRepeatMethod( (VGradient::VGradientRepeatMethod)m_dialog->gradientRepeat() );

	if( m_dialog->gradientFill() )
	{
		VFill fill;
		fill.gradient() = gradient;
		fill.setType( VFill::grad );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill ), true );
	}
	else
	{
		VStroke stroke;
		stroke.gradient() = gradient;
		stroke.setType( VStroke::grad );
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &stroke ), true );
	}

	view()->selectionChanged();
}

void
VGradientTool::mouseDragRelease( const KoPoint& current )
{
	// Y mirroring
	KoPoint fp = first();
	//fp.setY( -fp.y() + view()->canvasWidget()->viewport()->height() );
	KoPoint lp = last();
	//lp.setY( -lp.y() + view()->canvasWidget()->viewport()->height() );
	VGradient gradient;
	gradient.clearStops();
	gradient.addStop( VColor( m_dialog->startColor().rgb() ), 0.0, 0.5 );
	gradient.addStop( VColor( m_dialog->endColor().rgb() ), 1.0, 0.5 );
	gradient.setOrigin( fp );
	gradient.setVector( lp );
	gradient.setType( (VGradient::VGradientType)m_dialog->gradientType() );
	gradient.setRepeatMethod( (VGradient::VGradientRepeatMethod)m_dialog->gradientRepeat() );

	if( m_dialog->gradientFill() )
	{
		VFill fill;
		fill.gradient() = gradient;
		fill.setType( VFill::grad );
		view()->part()->addCommand(
			new VFillCmd( &view()->part()->document(), fill ), true );
	}
	else
		view()->part()->addCommand(
			new VStrokeCmd( &view()->part()->document(), &gradient ), true );

	view()->selectionChanged();
}

void
VGradientTool::showDialog() const
{
	m_dialog->exec();
}
