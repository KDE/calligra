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
#include <kcombobox.h>
#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vspiral.h>
#include "vspiraltool.h"
#include "KoUnitWidgets.h"

#include <kactioncollection.h>


VSpiralTool::VSpiralOptionsWidget::VSpiralOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialogBase( KDialogBase::Plain, Qt::Dialog, parent, name, true, i18n( "Insert Spiral" ), Ok | Cancel )
	, m_part( part )
{
	Q3GroupBox *group = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	new QLabel( i18n( "Type:" ), group );
	m_type = new KComboBox( false, group );
	m_type->insertItem( 0, i18n( "Round" ) );
	m_type->insertItem( 1, i18n( "Rectangular" ) );

	new QLabel( i18n( "Radius:" ), group );
	m_radius = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 50.0, KoUnit::U_MM );
	refreshUnit();
	new QLabel( i18n( "Segments:" ), group );
	m_segments = new KIntSpinBox( group );
	m_segments->setMinimum( 1 );
	new QLabel( i18n( "Fade:" ), group );
	m_fade = new KDoubleNumInput( group );
	m_fade->setRange( 0.0, 1.0, 0.05 );

	new QLabel( i18n( "Orientation:" ), group );
	m_clockwise = new KComboBox( false, group );
	m_clockwise->insertItem( 0, i18n( "Clockwise" ) );
	m_clockwise->insertItem( 1, i18n( "Counter Clockwise" ) );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	//setFixedSize( baseSize() );
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
	return m_clockwise->currentIndex() == 0;
}

uint
VSpiralTool::VSpiralOptionsWidget::type() const
{
	return m_type->currentIndex();
}

void
VSpiralTool::VSpiralOptionsWidget::setRadius( double value )
{
	m_radius->changeValue( value );
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
	m_clockwise->setCurrentIndex( value ? 0 : 1 );
}

void
VSpiralTool::VSpiralOptionsWidget::refreshUnit()
{
	m_radius->setUnit( m_part->unit() );
}

VSpiralTool::VSpiralTool( KarbonView *view )
	: VShapeTool( view, "tool_spiral", true )
{
	// create config dialog:
	m_optionsWidget = new VSpiralOptionsWidget( view->part() );
	m_optionsWidget->setSegments( 8 );
	m_optionsWidget->setFade( 0.8 );
	m_optionsWidget->setClockwise( true );
	registerTool( this );
}

void
VSpiralTool::arrowKeyReleased( Qt::Key key )
{
	int change = 0;
	if( key == Qt::Key_Up )
		change = 1;
	else if( key == Qt::Key_Down )
		change = -1;

	if( change != 0 )
	{
		draw();

		m_optionsWidget->setSegments( m_optionsWidget->segments() + change );

		draw();
	}
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

VPath*
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
				m_d2, (VSpiral::VSpiralType)m_optionsWidget->type() );
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
				m_d2, (VSpiral::VSpiralType)m_optionsWidget->type() );
}

bool
VSpiralTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VSpiralTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_spiral" ), i18n( "Spiral Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::SHIFT+Qt::Key_H );
		m_action->setToolTip( i18n( "Spiral" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO needs porting: m_action->setExclusiveGroup( "shapes" );
		//m_ownAction = true;
	}
}

