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


VStarOptionsWidget::VStarOptionsWidget( KarbonPart*part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part(part)
{
	// add width/height-input:
	m_outerRLabel = new QLabel( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName()), this );
	m_outerR = new KDoubleNumInput(0,this);
	m_outerR->setRange(0, 1000, 0.1);

	m_innerRLabel = new QLabel( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName()), this );
	m_innerR = new KDoubleNumInput(0,this);
	m_innerR->setRange(0, 1000, 0.1);

	new QLabel( i18n( "Edges:" ), this );
	m_edges = new KIntSpinBox( this );
	m_edges->setMinValue( 3 );
}

double
VStarOptionsWidget::innerR() const
{
	return KoUnit::ptFromUnit( m_innerR->value(),m_part->getUnit() ) ;

}

double
VStarOptionsWidget::outerR() const
{
	return KoUnit::ptFromUnit( m_outerR->value(),m_part->getUnit() ) ;
}

uint
VStarOptionsWidget::edges() const
{
	return m_edges->value();
}

void
VStarOptionsWidget::setInnerR( double value )
{
	m_innerR->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VStarOptionsWidget::setOuterR( double value )
{
	m_outerR->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VStarOptionsWidget::setEdges( uint value )
{
	m_edges->setValue( value );
}

void VStarOptionsWidget::refreshUnit ()
{
	m_outerRLabel->setText( i18n( "Outer Radius(%1):" ).arg(m_part->getUnitName() ) );
	m_innerRLabel->setText( i18n( "Inner Radius(%1):" ).arg(m_part->getUnitName() ) );
}

VStarTool::VStarTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Star" ), true )
{
	// create config dialog:
	m_optionsWidget = new VStarOptionsWidget(view->part());
	m_optionsWidget->setOuterR( 100.0 );
	m_optionsWidget->setInnerR( 50.0 );
	m_optionsWidget->setEdges( 5 );
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
				m_optionsWidget->outerR(),
				m_optionsWidget->innerR(),
				m_optionsWidget->edges() );
	}
	else
		return
			new VStar(
				0L,
				m_p,
				m_d1,
				m_optionsWidget->innerR() * m_d1 / m_optionsWidget->outerR(),
				m_optionsWidget->edges(),
				m_d2 );
}

#include "vstartool.moc"
