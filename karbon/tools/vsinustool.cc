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
#include <QLayout>
#include <kicon.h>

#include <klocale.h>
#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vsinus.h>
#include "vsinustool.h"
#include "KoUnitWidgets.h"

#include <kactioncollection.h>

VSinusTool::VSinusOptionsWidget::VSinusOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialog( parent )
	, m_part( part )
{
	setObjectName( name );
	setModal( true );
	setCaption( i18n( "Insert Sinus" ) );
	setButtons( Ok | Cancel );

	Q3GroupBox *group = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), group );
	m_width = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );
	m_heightLabel = new QLabel( i18n( "Height:" ), group );
	m_height = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );

	refreshUnit();

	new QLabel( i18n( "Periods:" ), group );
	m_periods = new KIntSpinBox( group );
	m_periods->setMinimum( 1 );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	setFixedSize( baseSize() );
}

double
VSinusTool::VSinusOptionsWidget::width() const
{
	return m_width->value();
}

double
VSinusTool::VSinusOptionsWidget::height() const
{
	return m_height->value();
}

uint
VSinusTool::VSinusOptionsWidget::periods() const
{
	return m_periods->value();
}

void
VSinusTool::VSinusOptionsWidget::setWidth( double value )
{
	m_width->changeValue( value );
}

void
VSinusTool::VSinusOptionsWidget::setHeight( double value )
{
	m_height->changeValue( value );
}

void
VSinusTool::VSinusOptionsWidget::setPeriods( uint value )
{
	m_periods->setValue( value );
}

void
VSinusTool::VSinusOptionsWidget::refreshUnit ()
{
	m_width->setUnit( m_part->unit() );
	m_height->setUnit( m_part->unit() );
}

VSinusTool::VSinusTool( KarbonView *view )
	: VShapeTool( view, "tool_sinus" )
{
	// create config widget:
	m_optionsWidget = new VSinusOptionsWidget( view->part() );
	m_optionsWidget->setPeriods( 1 );
	registerTool( this );
}

VSinusTool::~VSinusTool()
{
	delete( m_optionsWidget );
}

void
VSinusTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VPath*
VSinusTool::shape( bool interactive ) const
{
	if( interactive )
		return
			new VSinus(
				0L,
				m_p,
				m_optionsWidget->width(),
				m_optionsWidget->height(),
				m_optionsWidget->periods() );
	else
		return
			new VSinus(
				0L,
				m_p,
				m_d1,
				m_d2,
				m_optionsWidget->periods() );
}

bool
VSinusTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

void
VSinusTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_sinus" ), i18n( "Sinus Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::SHIFT+Qt::Key_S );
		m_action->setToolTip( i18n( "Sinus" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO needs porting: m_action->setExclusiveGroup( "shapes" );
		//m_ownAction = true;
	}
}

