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
#include "vstar.h"
#include "vstartool.h"
#include "vunitspinbox.h"


VStarTool::VStarOptionsWidget::VStarOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	// add width/height-input:
	m_outerRLabel = new QLabel( i18n( "Outer radius:" ), this );
	m_outerR = new VUnitDoubleSpinBox( 0.0, 1000.0, 0.5, 0.0, 2, this );

	m_innerRLabel = new QLabel( i18n( "Inner radius:" ), this );
	m_innerR = new VUnitDoubleSpinBox( 0.0, 1000.0, 0.5, 0.0, 2, this );

	refreshUnit();

	new QLabel( i18n( "Edges:" ), this );
	m_edges = new KIntSpinBox( this );
	m_edges->setMinValue( 3 );
	
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

void VStarTool::VStarOptionsWidget::refreshUnit()
{
	m_outerR->setUnit( m_part->getUnit() );
	m_innerR->setUnit( m_part->getUnit() );
}

VStarTool::VStarTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Star" ), true )
{
	// create config dialog:
	m_optionsWidget = new VStarOptionsWidget( view->part() );
	m_optionsWidget->m_outerR->setValue( KoUnit::ptToUnit( 100.0, view->part()->getUnit() ) );
	m_optionsWidget->m_innerR->setValue( KoUnit::ptToUnit( 50.0, view->part()->getUnit() ) );
	m_optionsWidget->m_edges->setValue( 5 );
}

void VStarTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VStarTool::~VStarTool()
{
	delete( m_optionsWidget );
}

VComposite*
VStarTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VStar(
				0L,
				m_p,
				KoUnit::ptFromUnit( m_optionsWidget->m_outerR->value(), view()->part()->getUnit() ),
				KoUnit::ptFromUnit( m_optionsWidget->m_innerR->value(), view()->part()->getUnit() ),
				m_optionsWidget->m_edges->value() );
	}
	else
		return
			new VStar(
				0L,
				m_p,
				m_d1,
				KoUnit::ptFromUnit( m_optionsWidget->m_innerR->value(), view()->part()->getUnit() ) * m_d1 /
				KoUnit::ptFromUnit( m_optionsWidget->m_outerR->value(), view()->part()->getUnit() ),
				m_optionsWidget->m_edges->value(),
				m_d2 );
}

