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


#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>
#include <knuminput.h>

#include "karbon_view.h"
#include "vsinus.h"
#include "vsinustool.h"
#include "karbon_part.h"


VSinusTool::VSinusOptionsWidget::VSinusOptionsWidget( KarbonPart *part,QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part(part)
{
	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), this );
	m_width = new KDoubleNumInput( 0, this );
	m_width->setSuffix( m_part->getUnitName() );
	m_heightLabel = new QLabel( i18n( "Height:" ), this );
	m_height = new KDoubleNumInput( 0, this );
	m_height->setSuffix( m_part->getUnitName() );
	new QLabel( i18n( "Periods:" ), this );
	m_periods = new KIntSpinBox( this );
	m_periods->setMinValue( 1 );
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VSinusTool::VSinusOptionsWidget::width() const
{
	return KoUnit::ptFromUnit( m_width->value(), m_part->getUnit() ) ;
}

double
VSinusTool::VSinusOptionsWidget::height() const
{
	return KoUnit::ptFromUnit( m_height->value(), m_part->getUnit() ) ;
}

uint
VSinusTool::VSinusOptionsWidget::periods() const
{
	return m_periods->value();
}

void
VSinusTool::VSinusOptionsWidget::setWidth( double value )
{
	m_width->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VSinusTool::VSinusOptionsWidget::setHeight( double value )
{
	m_height->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VSinusTool::VSinusOptionsWidget::setPeriods( uint value )
{
	m_periods->setValue( value );
}

void
VSinusTool::VSinusOptionsWidget::refreshUnit ()
{
	m_width->setSuffix( m_part->getUnitName() );
	m_height->setSuffix( m_part->getUnitName() );
}

VSinusTool::VSinusTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Sinus" ) )
{
	// create config widget:
	m_optionsWidget = new VSinusOptionsWidget(view->part());
	m_optionsWidget->setWidth( 100.0 );
	m_optionsWidget->setHeight( 100.0 );
	m_optionsWidget->setPeriods( 1 );
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

VComposite*
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

#include "vsinustool.moc"
