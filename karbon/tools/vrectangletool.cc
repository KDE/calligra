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
#include "vrectangle.h"
#include "vrectangletool.h"


VRectangleOptionsWidget::VRectangleOptionsWidget( KarbonPart*part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part(part)
{
	// add width/height-input:
	m_widthLabel =new QLabel( i18n( "Width(%1):" ).arg(m_part->getUnitName()), this );
	m_width = new KDoubleNumInput( 0, this );
	m_heightLabel =new QLabel( i18n( "Height(%1):" ).arg(m_part->getUnitName()), this );
	m_height = new KDoubleNumInput( 0, this );
	
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VRectangleOptionsWidget::width() const
{
	return KoUnit::ptFromUnit(m_width->value(),m_part->getUnit()) ;
}

double
VRectangleOptionsWidget::height() const
{
	return KoUnit::ptFromUnit(m_height->value(),m_part->getUnit()) ;
}

void
VRectangleOptionsWidget::setWidth( double value )
{
	m_width->setValue(KoUnit::ptToUnit( value, m_part->getUnit() ));
}

void
VRectangleOptionsWidget::setHeight( double value )
{
	m_height->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void VRectangleOptionsWidget::refreshUnit ()
{
	m_widthLabel->setText(i18n( "Width(%1):" ).arg(m_part->getUnitName()));
	m_heightLabel->setText( i18n( "Height(%1):" ).arg(m_part->getUnitName()));
}

VRectangleTool::VRectangleTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Rectangle" ) )
{
	// Create config dialog:
	m_optionsWidget = new VRectangleOptionsWidget( view->part() );
	m_optionsWidget->setWidth( 100.0 );
	m_optionsWidget->setHeight( 100.0 );
}

VRectangleTool::~VRectangleTool()
{
	delete( m_optionsWidget );
}

void VRectangleTool::refreshUnit()
{
    m_optionsWidget->refreshUnit();
}

VComposite*
VRectangleTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VRectangle(
				0L,
				m_p,
				m_optionsWidget->width(),
				m_optionsWidget->height() );
	}
	else
		return
			new VRectangle(
				0L,
				m_p,
				m_d1,
				m_d2 );
}

#include "vrectangletool.moc"
