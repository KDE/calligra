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
#include <koUnitWidgets.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vstar.h>
#include "vpolygontool.h"

VPolygonTool::VPolygonOptionsWidget::VPolygonOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name ), m_part( part )
{
	new QLabel( i18n( "Radius:" ), this );
	m_radius = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5, 50.0, KoUnit::U_MM );
	refreshUnit();
	new QLabel( i18n( "Edges:" ), this );
	m_edges = new KIntSpinBox( this );
	m_edges->setMinValue( 3 );
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VPolygonTool::VPolygonOptionsWidget::radius() const
{
	return m_radius->value();
}

uint
VPolygonTool::VPolygonOptionsWidget::edges() const
{
	return m_edges->value();
}

void
VPolygonTool::VPolygonOptionsWidget::setRadius( double value )
{
	m_radius->setValue( value );
}

void
VPolygonTool::VPolygonOptionsWidget::setEdges( uint value )
{
	m_edges->setValue( value );
}

void
VPolygonTool::VPolygonOptionsWidget::refreshUnit()
{
	m_radius->setUnit( m_part->unit() );
}

VPolygonTool::VPolygonTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Polygon" ), true )
{
	// create config dialog:
	m_optionsWidget = new VPolygonOptionsWidget( view->part() );
	m_optionsWidget->setEdges( 5 );
	registerTool( this );
}

VPolygonTool::~VPolygonTool()
{
	delete( m_optionsWidget );
}

void
VPolygonTool::refreshUnit()
{
	m_optionsWidget->refreshUnit();
}

void
VPolygonTool::arrowKeyReleased( Qt::Key key )
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
VPolygonTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VStar(
				0L,
				m_p,
				KoUnit::ptFromUnit( m_optionsWidget->radius(), view()->part()->unit() ),
				KoUnit::ptFromUnit( m_optionsWidget->radius(), view()->part()->unit() ),
				m_optionsWidget->edges(), 0, 0, 0, VStar::polygon );
	}
	else
		return
			new VStar(
				0L,
				m_p,
				m_d1, m_d1,
				m_optionsWidget->edges(),
				m_d2, 0, 0, VStar::polygon );
}

