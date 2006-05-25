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


#include <q3groupbox.h>
#include <QLabel>

#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vrectangle.h>
#include "vroundrecttool.h"
#include "KoUnitWidgets.h"

#include <kgenericfactory.h>

VRoundRectTool::VRoundRectOptionsWidget::VRoundRectOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Round Rect" ), Ok | Cancel ), m_part( part )
{
	Q3GroupBox *group = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	new QLabel( i18n( "Width:" ), group );
	
	KoUnit::Unit unit = KoUnit::U_CM;
	m_width = new KoUnitDoubleSpinBox( group, 0.0, KoUnit::fromUserValue( 1000.0, unit ), KoUnit::fromUserValue( 0.5, unit ), KoUnit::fromUserValue( 10.0, unit ), unit );

	new QLabel( i18n( "Height (%1):", KoUnit::unitName( m_part->unit() )), group );
	m_height = new KoUnitDoubleSpinBox( group, 0.0, KoUnit::fromUserValue( 1000.0, unit ), KoUnit::fromUserValue( 0.5, unit ), KoUnit::fromUserValue( 10.0, unit ), unit );

	new QLabel( i18n( "Edge radius X:" ), group );
	m_roundx = new KoUnitDoubleSpinBox( group, 0.0, KoUnit::fromUserValue( 100.0, unit ), KoUnit::fromUserValue( 0.1, unit ), KoUnit::fromUserValue( 1.0, unit ), unit );

	new QLabel( i18n( "Edge radius Y:" ), group );
	m_roundy = new KoUnitDoubleSpinBox( group, 0.0, KoUnit::fromUserValue( 100.0, unit ), KoUnit::fromUserValue( 0.1, unit ), KoUnit::fromUserValue( 1.0, unit ), unit );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	setFixedSize( baseSize() );
}

double
VRoundRectTool::VRoundRectOptionsWidget::width() const
{
	return m_width->value();
}

double
VRoundRectTool::VRoundRectOptionsWidget::height() const
{
	return m_height->value();
}

double
VRoundRectTool::VRoundRectOptionsWidget::roundx() const
{
	return m_roundx->value();
}

double
VRoundRectTool::VRoundRectOptionsWidget::roundy() const
{
	return m_roundy->value();
}

void
VRoundRectTool::VRoundRectOptionsWidget::setWidth( double value )
{
	m_width->changeValue( value );
}

void
VRoundRectTool::VRoundRectOptionsWidget::setHeight( double value )
{
	m_height->changeValue( value );
}

void
VRoundRectTool::VRoundRectOptionsWidget::setRoundX( double value )
{
	m_roundx->changeValue( value );
}

void
VRoundRectTool::VRoundRectOptionsWidget::setRoundY( double value )
{
	m_roundy->changeValue( value );
}

void
VRoundRectTool::VRoundRectOptionsWidget::refreshUnit ()
{
	m_width->setUnit( m_part->unit() );
	m_height->setUnit( m_part->unit() );
	m_roundx->setUnit( m_part->unit() );
	m_roundy->setUnit( m_part->unit() );
}

VRoundRectTool::VRoundRectTool( KarbonView *view )
		: VShapeTool( view, "tool_round_rectangle" )
{
	// Create config dialog:
	m_optionsWidget = new VRoundRectOptionsWidget( view->part() );
	registerTool( this );
}

VRoundRectTool::~VRoundRectTool()
{
	delete( m_optionsWidget );
}

void VRoundRectTool::refreshUnit()
{
	m_optionsWidget->refreshUnit();
}

VPath*
VRoundRectTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VRectangle(
				0L,
				m_p,
				m_optionsWidget->width(),
				m_optionsWidget->height(),
				m_optionsWidget->roundx(),
				m_optionsWidget->roundy() );
	}
	else {
		return
			new VRectangle(
				0L,
				m_p,
				m_d1,
				m_d2,
				m_optionsWidget->roundx(),
				m_optionsWidget->roundy() );
	}
}

bool
VRoundRectTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VRoundRectTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KRadioAction( i18n( "Round Rectangle Tool" ), "14_roundrect", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Round Rectangle" ) );
		m_action->setExclusiveGroup( "shapes" );
		//m_ownAction = true;
	}
}

