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
#include <kcombobox.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vstar.h>
#include "vstartool.h"
#include "koUnitWidgets.h"


VStarOptionsWidget::VStarOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	new QLabel( i18n( "Type:" ), this );
	m_type = new KComboBox( false, this );
	m_type->insertItem( i18n( "Star outline" ), 0 );
	m_type->insertItem( i18n( "Spoke" ), 1 );
	m_type->insertItem( i18n( "Wheel" ), 2 );
	m_type->insertItem( i18n( "Polygon" ), 3 );
	m_type->insertItem( i18n( "Framed Star" ), 4 );
	connect( m_type, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

	// add width/height-input:
	m_outerRLabel = new QLabel( i18n( "Outer radius:" ), this );
	m_outerR = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5, 50.0, KoUnit::U_MM );

	m_innerRLabel = new QLabel( i18n( "Inner radius:" ), this );
	m_innerR = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5, 25.0, KoUnit::U_MM );

	typeChanged( 0 );

	refreshUnit();

	new QLabel( i18n( "Edges:" ), this );
	m_edges = new KIntSpinBox( this );
	m_edges->setMinValue( 3 );

	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

void
VStarOptionsWidget::refreshUnit()
{
	m_outerR->setUnit( m_part->unit() );
	m_innerR->setUnit( m_part->unit() );
}

void
VStarOptionsWidget::setEdges( int v )
{
	m_edges->setValue( v );
}

void 
VStarOptionsWidget::setInnerRadius( double v )
{
	m_innerR->changeValue( v );
}

void
VStarOptionsWidget::setOuterRadius( double v )
{
	m_outerR->changeValue( v );
}

int
VStarOptionsWidget::edges() const
{
	return m_edges->value();
}

double 
VStarOptionsWidget::innerRadius() const
{
	return m_innerR->value();
}

double
VStarOptionsWidget::outerRadius() const
{
	return m_outerR->value();
}

uint
VStarOptionsWidget::type() const
{
	return m_type->currentItem();
}

void
VStarOptionsWidget::typeChanged( int type )
{
	kdDebug() << "type : " << type << endl;
	m_innerR->setEnabled( type == VStar::star_outline || type == VStar::framed_star );
}

VStarTool::VStarTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Star" ), true )
{
	// create config dialog:
	m_optionsWidget = new VStarOptionsWidget( view->part() );
	m_optionsWidget->setEdges( 5 );
	registerTool( this );
}

void VStarTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VStarTool::~VStarTool()
{
	delete( m_optionsWidget );
}

void
VStarTool::arrowKeyReleased( Qt::Key key )
{
	int change = 0;
	if( key == Qt::Key_Up )
		change = 1;
	else if( key == Qt::Key_Down )
		change = -1;

	if( change != 0 )
	{
		draw();

		m_optionsWidget->setEdges( m_optionsWidget->edges() + change );

		draw();
	}
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
				KoUnit::ptFromUnit( m_optionsWidget->outerRadius(), view()->part()->unit() ),
				KoUnit::ptFromUnit( m_optionsWidget->innerRadius(), view()->part()->unit() ),
				m_optionsWidget->edges(), 0.0, (VStar::VStarType)m_optionsWidget->type() );
	}
	else
		return
			new VStar(
				0L,
				m_p,
				m_d1,
				m_optionsWidget->innerRadius() * m_d1 /
				m_optionsWidget->outerRadius(),
				m_optionsWidget->edges(),
				m_d2, (VStar::VStarType)m_optionsWidget->type() );
}

#include "vstartool.moc"
