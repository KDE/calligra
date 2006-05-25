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

#include <QLabel>
#include <q3groupbox.h>

#include <klocale.h>
#include <knuminput.h>
#include <kcombobox.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vstar.h>
#include "vstartool.h"
#include "KoUnitWidgets.h"


VStarOptionsWidget::VStarOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Star" ), Ok | Cancel ), m_part( part )
{
	Q3GroupBox *group = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	new QLabel( i18n( "Type:" ), group );
	m_type = new KComboBox( false, group );
	m_type->insertItem( i18n( "Star Outline" ), VStar::star_outline );
	m_type->insertItem( i18n( "Spoke" ), VStar::spoke );
	m_type->insertItem( i18n( "Wheel" ), VStar::wheel );
	m_type->insertItem( i18n( "Polygon" ), VStar::polygon );
	m_type->insertItem( i18n( "Framed Star" ), VStar::framed_star);
	m_type->insertItem( i18n( "Star" ), VStar::star );
	m_type->insertItem( i18n( "Gear" ), VStar::gear );
	connect( m_type, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

	// add width/height-input:
	m_outerRLabel = new QLabel( i18n( "Outer radius:" ), group );
	m_outerR = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 50.0, KoUnit::U_MM );
	connect( m_outerR, SIGNAL( valueChanged( double ) ), this, SLOT( setOuterRadius( double ) ) );

	m_innerRLabel = new QLabel( i18n( "Inner radius:" ), group );
	m_innerR = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 25.0, KoUnit::U_MM );

	refreshUnit();

	new QLabel( i18n( "Edges:" ), group );
	m_edges = new KIntSpinBox( group );
	m_edges->setMinValue( 3 );
	connect( m_edges, SIGNAL( valueChanged( int ) ), this, SLOT( setEdges( int ) ) );

	new QLabel( i18n( "Inner angle:" ), group );
	m_innerAngle = new KIntSpinBox( group );
	m_innerAngle->setMinValue( 0 );
	m_innerAngle->setMaxValue( 360 );

	new QLabel( i18n( "Roundness:" ), group );
	m_roundness = new KDoubleNumInput( group );
	m_roundness->setRange( 0.0, 1.0, 0.05 );

	typeChanged( VStar::star_outline );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	setFixedSize( baseSize() );
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

	// set optimal inner radius
	if( type() == VStar::star )
		m_innerR->setValue( VStar::getOptimalInnerRadius( edges(), outerRadius(), innerAngle() ) );
}

void
VStarOptionsWidget::setInnerRadius( double v )
{
	m_innerR->changeValue( v );
}

void
VStarOptionsWidget::setOuterRadius( double v )
{
	m_outerR->setValue( v );

	// set optimal inner radius
	if( type() == VStar::star )
		m_innerR->setValue( VStar::getOptimalInnerRadius( edges(), outerRadius(), innerAngle() ) );
}

double
VStarOptionsWidget::roundness() const
{
	return m_roundness->value();
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

uint
VStarOptionsWidget::innerAngle() const
{
	return m_innerAngle->value();
}

void
VStarOptionsWidget::typeChanged( int type )
{
	m_innerR->setEnabled( type == VStar::star || type == VStar::star_outline || type == VStar::framed_star || type == VStar::gear );
	m_innerAngle->setEnabled( type == VStar::star || type == VStar::star_outline || type == VStar::framed_star || type == VStar::gear );

	// set optimal inner radius
	if( type == VStar::star )
		m_innerR->changeValue( VStar::getOptimalInnerRadius( edges(), outerRadius(), innerAngle() ) );
}

VStarTool::VStarTool( KarbonView *view )
	: VShapeTool( view, "tool_star", true )
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

VPath*
VStarTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VStar(
				0L,
				m_p,
				m_optionsWidget->outerRadius(),
				m_optionsWidget->innerRadius(),
				m_optionsWidget->edges(), 0.0, m_optionsWidget->innerAngle(),
				m_optionsWidget->roundness(), (VStar::VStarType)m_optionsWidget->type() );
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
				m_d2, m_optionsWidget->innerAngle(),
				m_optionsWidget->roundness(), (VStar::VStarType)m_optionsWidget->type() );
}

bool
VStarTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VStarTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		KShortcut shortcut( Qt::Key_Plus );
		shortcut.append(KShortcut( Qt::Key_F9 ) );
		m_action = new KRadioAction( i18n( "Star Tool" ), "14_star", shortcut, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Draw a star" ) );
		m_action->setExclusiveGroup( "shapes" );
		//m_ownAction = true;
	}
}

#include "vstartool.moc"
