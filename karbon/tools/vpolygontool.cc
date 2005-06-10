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


#include <qlabel.h>
#include <qgroupbox.h>

#include <kdialogbase.h>
#include <klocale.h>
#include <koUnitWidgets.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include <shapes/vstar.h>
#include "vpolygontool.h"

VPolygonTool::VPolygonOptionsWidget::VPolygonOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Polygon" ), Ok | Cancel ), m_part(part)
{
	QGroupBox *group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	new QLabel( i18n( "Radius:" ), group );
	m_radius = new KDoubleSpinBox(0.0, 1000.0, 0.5, 5.0,2, group );
	refreshUnit();
	new QLabel( i18n( "Edges:" ), group );
	m_edges = new KIntSpinBox( group );
	m_edges->setMinValue( 3 );

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	//setFixedSize( baseSize() );
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
	m_radius->setSuffix( KoUnit::unitName( m_part->unit() ) );
}

VPolygonTool::VPolygonTool( KarbonPart *part )
	: VShapeTool( part, i18n( "Insert Polygon" ), true )
{
	// create config dialog:
	m_optionsWidget = new VPolygonOptionsWidget( part );
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

VPath*
VPolygonTool::shape( bool interactive ) const
{
	if( interactive )
	{
		return
			new VStar(
				0L,
				m_p,
				KoUnit::fromUserValue( m_optionsWidget->radius(), view()->part()->unit() ),
				KoUnit::fromUserValue( m_optionsWidget->radius(), view()->part()->unit() ),
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

bool
VPolygonTool::showDialog() const
{
	return m_optionsWidget->exec() == QDialog::Accepted;
}

