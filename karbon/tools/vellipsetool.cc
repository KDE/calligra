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

#include <klocale.h>
#include "koUnitWidgets.h"
#include <koUnit.h>
#include <kcombobox.h>
#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vellipse.h>
#include "vellipsetool.h"
#include "vglobal.h"


VEllipseOptionsWidget::VEllipseOptionsWidget( KarbonPart *part, QWidget *parent, const char *name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	new QLabel( i18n( "Type:" ), this );
	m_type = new KComboBox( false, this );
	m_type->insertItem( i18n( "Full" ), VEllipse::full );
	m_type->insertItem( i18n( "Section" ), VEllipse::section );
	m_type->insertItem( i18n( "Pie" ), VEllipse::cut );
	m_type->insertItem( i18n( "Arc" ), VEllipse::arc );
	connect( m_type, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), this );
	m_width = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );
	m_heightLabel = new QLabel( i18n( "Height:" ), this );
	m_height = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );

	new QLabel( i18n( "Start angle:" ), this );
	m_startAngle = new KIntSpinBox( this );
	m_startAngle->setMinValue( 0 );
	m_startAngle->setMaxValue( 360 );

	new QLabel( i18n( "End angle:" ), this );
	m_endAngle = new KIntSpinBox( this );
	m_endAngle->setMinValue( 0 );
	m_endAngle->setMaxValue( 360 );

	typeChanged( VEllipse::full );

	refreshUnit();
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

void
VEllipseOptionsWidget::typeChanged( int type )
{
	m_startAngle->setEnabled( type != VEllipse::full );
	m_endAngle->setEnabled( type != VEllipse::full );
}

uint
VEllipseOptionsWidget::type() const
{
	return m_type->currentItem();
}

uint
VEllipseOptionsWidget::startAngle() const
{
	return m_startAngle->value();
}

uint
VEllipseOptionsWidget::endAngle() const
{
	return m_endAngle->value();
}

double
VEllipseOptionsWidget::width() const
{
	return m_width->value();
}

double
VEllipseOptionsWidget::height() const
{
	return m_height->value();
}

void
VEllipseOptionsWidget::setWidth( double value )
{
	m_width->changeValue( value );
}

void
VEllipseOptionsWidget::setHeight( double value )
{
	m_height->changeValue( value );
}

void
VEllipseOptionsWidget::refreshUnit ()
{
	m_width->setUnit( m_part->unit() );
	m_height->setUnit( m_part->unit() );
}

VEllipseTool::VEllipseTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Ellipse" ) )
{
	// create config dialog:
	m_optionsWidget = new VEllipseOptionsWidget( view->part() );
	registerTool( this );

	m_startAngle = m_endAngle = 0;
	m_state = normal;
}

VEllipseTool::~VEllipseTool()
{
	delete( m_optionsWidget );
}

void
VEllipseTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VComposite*
VEllipseTool::shape( bool interactive ) const
{
	if( interactive )
	{
		double d1 = KoUnit::ptFromUnit( m_optionsWidget->width(), view()->part()->unit() ) / 2.0;
		double d2 = KoUnit::ptFromUnit( m_optionsWidget->height(), view()->part()->unit() ) / 2.0;
		return
			new VEllipse(
				0L,
				KoPoint( m_center.x() - d1, m_center.y() - d2 ),
				d1 * 2.0, d2 * 2.0,
				(VEllipse::VEllipseType)m_optionsWidget->type(),
				m_optionsWidget->startAngle(),
				m_optionsWidget->endAngle() );
	}
	else
		return
			new VEllipse(
				0L,
				KoPoint( m_center.x() - m_d1, m_center.y() - m_d2 ),
				m_d1 * 2.0,
				m_d2 * 2.0,
				(VEllipse::VEllipseType)m_optionsWidget->type(),
				m_startAngle, m_endAngle );
}

void
VEllipseTool::mouseMove()
{
	if( m_state == normal )
		return;

	draw();

	//recalc();

	if( m_state == startangle )
	{
		m_startAngle = atan2( last().y() - m_center.y(), last().x() - m_center.x() );
		m_startAngle = ( ( -VGlobal::pi_2 + m_startAngle ) / VGlobal::pi_2 ) * 90.0;
		if( m_startAngle < 0 )
			m_startAngle += 360.0;
	}
	else 
	{
		m_endAngle = atan2( last().y() - m_center.y(), last().x() - m_center.x() );
		m_endAngle = ( ( -VGlobal::pi_2 + m_endAngle ) / VGlobal::pi_2 ) * 90.0;
		if( m_endAngle < 0 )
			m_endAngle += 360.0;
	}

	draw();
}

void
VEllipseTool::mouseDragRelease()
{
	if( m_optionsWidget->type() == VEllipse::full )
		VShapeTool::mouseDragRelease();

	if( m_state == normal )
		if( m_optionsWidget->type() != VEllipse::full )
			m_state = startangle;
}

void
VEllipseTool::mouseButtonPress()
{
	if( m_state == normal )
	{
		VShapeTool::mouseButtonPress();
		m_center = first();
	}
}

void
VEllipseTool::mouseButtonRelease()
{
	if( m_optionsWidget->type() == VEllipse::full || m_state == normal )
		VShapeTool::mouseButtonRelease();

	if( m_state == startangle )
		m_state = endangle;
	else if( m_state == endangle )
	{
		VShapeTool::mouseDragRelease();
		m_startAngle = m_endAngle = 0;
		m_state = normal;
	}
}

#include "vellipsetool.moc"

