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
#include <kicon.h>

#include <klocale.h>
#include <knuminput.h>

#include <karbon_view.h>
#include <karbon_part.h>
#include "vrectangletool.h"
#include <KoUnitWidgets.h>

#include <kactioncollection.h>

VRectangleTool::VRectangleOptionsWidget::VRectangleOptionsWidget( KarbonPart *part, QWidget* parent, const char* name )
	: KDialog( parent )
	, m_part( part )
{
	setObjectName( name );
	setModal( true );
	setCaption( i18n( "Insert Rectangle" ) );
	setButtons( Ok | Cancel );

	Q3GroupBox *group = new Q3GroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	// add width/height-input:
	m_widthLabel = new QLabel( i18n( "Width:" ), group );
	m_width = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );

	m_heightLabel = new QLabel( i18n( "Height:" ), group );
	m_height = new KoUnitDoubleSpinBox( group, 0.0, 1000.0, 0.5, 100.0, KoUnit::U_MM );

	refreshUnit();

	group->setInsideMargin( 4 );
	group->setInsideSpacing( 2 );

	setMainWidget( group );
	//setFixedSize( baseSize() );
}

double
VRectangleTool::VRectangleOptionsWidget::width() const
{
	return m_width->value();
}

double
VRectangleTool::VRectangleOptionsWidget::height() const
{
	return m_height->value();
}

void
VRectangleTool::VRectangleOptionsWidget::setWidth( double value )
{
	m_width->setValue( value );
}

void
VRectangleTool::VRectangleOptionsWidget::setHeight( double value )
{
	m_height->setValue( value );
}

void
VRectangleTool::VRectangleOptionsWidget::refreshUnit()
{
	m_width->setUnit( m_part->unit() );
	m_height->setUnit( m_part->unit() );
}

VRectangleTool::VRectangleTool( KarbonView *view )
	: VShapeTool( view, "tool_rectangle" )
{
	// Create config dialog:
	m_optionWidget = new VRectangleOptionsWidget( view->part() );
	registerTool( this );
}

VRectangleTool::~VRectangleTool()
{
	delete( m_optionWidget );
}

void
VRectangleTool::refreshUnit()
{
	m_optionWidget->refreshUnit();
}

VPath *
VRectangleTool::shape( bool interactive ) const
{
#if 0
	if( interactive )
	{
		return
			new VRectangle(
				0L,
				m_p,
				m_optionWidget->width(),
				m_optionWidget->height() );
	}
	else
		return
			new VRectangle(
				0L,
				m_p,
				m_d1,
				m_d2 );
#endif
        return 0;
}

bool
VRectangleTool::showDialog() const
{
	return m_optionWidget->exec() == QDialog::Accepted;
}

void
VRectangleTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( objectName() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( KIcon( "14_rectangle" ), i18n( "Rectangle Tool" ), collection, objectName() );
		m_action->setDefaultShortcut( Qt::Key_Plus+Qt::Key_F9 );
		m_action->setToolTip( i18n( "Rectangle" ) );
		connect( m_action, SIGNAL( triggered() ), this, SLOT( activate() ) );
		// TODO needs porting: m_action->setExclusiveGroup( "shapes" );
		//m_ownAction = true;
	}
}

