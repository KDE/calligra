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
#include <knuminput.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vrectangle.h"
#include "vrectangletool.h"
#include "vunitspinbox.h"

VRectangleTool::VRectangleOptionsWidget::VRectangleOptionsWidget( KarbonPart*part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part(part)
{
	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), this );
	m_width = new VUnitDoubleSpinBox( 0.0, 1000.0, 0.5, 0.0, 2, this );
	m_heightLabel = new QLabel( i18n( "Height:" ), this );
	m_height = new VUnitDoubleSpinBox( 0.0, 1000.0, 0.5, 0.0, 2, this );

	refreshUnit();

	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VRectangleTool::VRectangleOptionsWidget::width() const
{
	return KoUnit::ptFromUnit( m_width->value(), m_part->getUnit() );
}

double
VRectangleTool::VRectangleOptionsWidget::height() const
{
	return KoUnit::ptFromUnit( m_height->value(), m_part->getUnit() );
}

void
VRectangleTool::VRectangleOptionsWidget::setWidth( double value )
{
	m_width->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VRectangleTool::VRectangleOptionsWidget::setHeight( double value )
{
	m_height->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VRectangleTool::VRectangleOptionsWidget::refreshUnit()
{
	m_width->setUnit( m_part->getUnit() );
	m_height->setUnit( m_part->getUnit() );
}

VRectangleTool::VRectangleTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Rectangle" ) )
{
	// Create config dialog:
	m_optionsWidget = new VRectangleOptionsWidget( view->part() );
	m_optionsWidget->setWidth( 100.0 );
	m_optionsWidget->setHeight( 100.0 );
}

VRectangleTool::~VRectangleTool()
{
	delete( m_optionsWidget );
}

void
VRectangleTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VComposite *
VRectangleTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VRectangle(
				0L,
				m_p,
				m_optionsWidget->width(),
				m_optionsWidget->height() );
	}
	else
		return
			new VRectangle(
				0L,
				m_p,
				m_d1,
				m_d2 );
}

