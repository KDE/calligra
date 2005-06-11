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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>
#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vsinus.h>
#include "vsinustool.h"
#include "koUnitWidgets.h"


VSinusTool::VSinusOptionsWidget::VSinusOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Sinus" ), Ok | Cancel ), m_part( part )
{
	QGroupBox *group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), group );
	m_width = new KoBuggyUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );
	m_heightLabel = new QLabel( i18n( "Height:" ), group );
	m_height = new KoBuggyUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );

	refreshUnit();

	new QLabel( i18n( "Periods:" ), group );
	m_periods = new KIntSpinBox( group );
	m_periods->setMinValue( 1 );

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
	m_width->setValue( value );
}

void
VSinusTool::VSinusOptionsWidget::setHeight( double value )
{
	m_height->setValue( value );
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

VSinusTool::VSinusTool( KarbonPart *part )
	: VShapeTool( part, i18n( "Insert Sinus" ) )
{
	// create config widget:
	m_optionsWidget = new VSinusOptionsWidget( part );
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
				KoUnit::fromUserValue( m_optionsWidget->width(), view()->part()->unit() ),
				KoUnit::fromUserValue( m_optionsWidget->height(), view()->part()->unit() ),
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

