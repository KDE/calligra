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

#include <qlabel.h>

#include <klocale.h>
#include <kcombobox.h>
#include <knuminput.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vspiral.h"
#include "vspiraltool.h"
#include "vunitspinbox.h"


VSpiralTool::VSpiralOptionsWidget::VSpiralOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	new QLabel( i18n( "Radius:" ), this );
	m_radius = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	refreshUnit();
	new QLabel( i18n( "Segments:" ), this );
	m_segments = new KIntSpinBox( this );
	m_segments->setMinValue( 1 );
	new QLabel( i18n( "Fade:" ), this );
	m_fade = new KDoubleNumInput( 0.0, this );
	m_fade->setRange( 0.0, 1.0, 0.05 );

	new QLabel( i18n( "Orientation:" ), this );
	m_clockwise = new KComboBox( false, this );
	m_clockwise->insertItem( i18n( "Clockwise" ), 0 );
	m_clockwise->insertItem( i18n( "Counter Clockwise" ), 1 );
	
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VSpiralTool::VSpiralOptionsWidget::radius() const
{
	return m_radius->value();
}

uint
VSpiralTool::VSpiralOptionsWidget::segments() const
{
	return m_segments->value();
}

double
VSpiralTool::VSpiralOptionsWidget::fade() const
{
	return m_fade->value();
}

bool
VSpiralTool::VSpiralOptionsWidget::clockwise() const
{
	return m_clockwise->currentItem() == 0;
}

void
VSpiralTool::VSpiralOptionsWidget::setRadius( double value )
{
	m_radius->setValue( value );
}

void
VSpiralTool::VSpiralOptionsWidget::setSegments( uint value )
{
	m_segments->setValue( value );
}

void
VSpiralTool::VSpiralOptionsWidget::setFade( double value )
{
	m_fade->setValue( value );
}

void
VSpiralTool::VSpiralOptionsWidget::setClockwise( bool value )
{
	m_clockwise->setCurrentItem( value ? 0 : 1 );
}

void
VSpiralTool::VSpiralOptionsWidget::refreshUnit()
{
	m_radius->setUnit( m_part->unit() );
}

VSpiralTool::VSpiralTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Spiral" ), true )
{
	// create config dialog:
	m_optionsWidget = new VSpiralOptionsWidget( view->part() );
	m_optionsWidget->setRadius( 100.0 );
	m_optionsWidget->setSegments( 8 );
	m_optionsWidget->setFade( 0.8 );
	m_optionsWidget->setClockwise( true );
}

VSpiralTool::~VSpiralTool()
{
	delete( m_optionsWidget );
}

void
VSpiralTool::refreshUnit()
{
	m_optionsWidget->refreshUnit();
}

VComposite*
VSpiralTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VSpiral(
				0L,
				m_p,
				m_optionsWidget->radius(),
				m_optionsWidget->segments(),
				m_optionsWidget->fade(),
				m_optionsWidget->clockwise(),
				m_d2 );
	}
	else
		return
			new VSpiral(
				0L,
				m_p,
				m_d1,
				m_optionsWidget->segments(),
				m_optionsWidget->fade(),
				m_optionsWidget->clockwise(),
				m_d2 );
}

