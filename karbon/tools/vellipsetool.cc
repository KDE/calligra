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

#include <klocale.h>
#include "vunitspinbox.h"
#include <koUnit.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vellipse.h"
#include "vellipsetool.h"


VEllipseTool::VEllipseOptionsWidget::VEllipseOptionsWidget( KarbonPart *part, QWidget *parent, const char *name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), this );
	m_width = new KoUnitDoubleComboBox( this, 0.0, 1000.0 );
	m_width->setValueInUnit( 100.0, KoUnit::U_PT );
	m_heightLabel = new QLabel( i18n( "Height:" ), this );
	m_height = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	m_height->setValueInUnit( 100.0, KoUnit::U_MM );
	refreshUnit();
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VEllipseTool::VEllipseOptionsWidget::width() const
{
	return m_width->value();
}

double
VEllipseTool::VEllipseOptionsWidget::height() const
{
	return m_height->value();
}

void
VEllipseTool::VEllipseOptionsWidget::setWidth( double value )
{
	m_width->setValue( value );
}

void
VEllipseTool::VEllipseOptionsWidget::setHeight( double value )
{
	m_height->setValue( value );
}

void
VEllipseTool::VEllipseOptionsWidget::refreshUnit ()
{
	m_width->setUnit( m_part->unit() );
	m_height->setUnit( m_part->unit() );
}

VEllipseTool::VEllipseTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Ellipse" ) )
{
	// create config dialog:
	m_optionsWidget = new VEllipseOptionsWidget( view->part() );
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
		return
			new VEllipse(
				0L,
				m_p,
				m_optionsWidget->width(),
				m_optionsWidget->height() );
	}
	else
		return
			new VEllipse(
				0L,
				m_p,
				m_d1,
				m_d2 );
}

