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
#include "vpolygon.h"
#include "vpolygontool.h"

VPolygonOptionsWidget::VPolygonOptionsWidget( QWidget* parent, const char* name )
	: QGroupBox( 2, Qt::Horizontal, 0L, parent, name )
{
	new QLabel( i18n( "Radius:" ), this );
	m_radius = new KDoubleNumInput( 0, this );
	new QLabel( i18n( "Edges:" ), this );
	m_edges = new KIntSpinBox( this );
	m_edges->setMinValue( 3 );
	setInsideMargin( 4 );
	setInsideSpacing( 2 );
}

double
VPolygonOptionsWidget::radius() const
{
	return m_radius->value();
}

uint
VPolygonOptionsWidget::edges() const
{
	return m_edges->value();
}

void
VPolygonOptionsWidget::setRadius( double value )
{
	m_radius->setValue( value );
}

void
VPolygonOptionsWidget::setEdges( uint value )
{
	m_edges->setValue( value );
}

VPolygonTool::VPolygonTool( KarbonView* view )
	: VShapeTool( view, i18n( "Insert Polygon" ), true )
{
	// create config dialog:
	m_optionsWidget = new VPolygonOptionsWidget();
	m_optionsWidget->setRadius( 100.0 );
	m_optionsWidget->setEdges( 5 );
}

VPolygonTool::~VPolygonTool()
{
	delete( m_optionsWidget );
}

VComposite*
VPolygonTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VPolygon(
				0L,
				m_p,
				m_optionsWidget->radius(),
				m_optionsWidget->edges() );
	}
	else
		return
			new VPolygon(
				0L,
				m_p,
				m_d1,
				m_optionsWidget->edges(),
				m_d2 );
}

#include "vpolygontool.moc"
