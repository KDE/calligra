/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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

#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <kiconloader.h>
#include <klocale.h>
#include <koMainWindow.h>

#include "koUnitWidgets.h"

#include "karbon_part.h"
#include "karbon_view.h"
#include "vstroke.h"
#include "vcolorslider.h"
#include "vselection.h"
#include "vstrokecmd.h"

#include "vstrokedocker.h"

VStrokeDocker::VStrokeDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Stroke Properties" ) );

	QPushButton *button;
	mainWidget = new QWidget( this );
	QGridLayout *mainLayout = new QGridLayout( mainWidget, 4, 2 );
	
	QLabel* widthLabel = new QLabel( i18n ( "Width:" ), mainWidget );
	mainLayout->addWidget( widthLabel, 0, 0 );
	m_setLineWidth = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 1 );
	mainLayout->addWidget ( m_setLineWidth, 0, 1 );
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	
	QLabel* capLabel = new QLabel( i18n ( "Cap:" ), mainWidget );
	mainLayout->addWidget( capLabel, 1, 0 );
	m_capGroup = new QHButtonGroup( mainWidget );
	m_capGroup->setFrameShape( NoFrame );
	m_capGroup->setInsideMargin( 1 );
	m_capGroup->setExclusive( true );
	button = new QPushButton( "", m_capGroup );
	button->setPixmap( SmallIcon( "cap_butt" ) );
	button->setToggleButton( true );
	m_capGroup->insert( button );
	button = new QPushButton( "", m_capGroup );
	button->setPixmap( SmallIcon( "cap_round" ) );
	button->setToggleButton( true );
	m_capGroup->insert( button );
	button = new QPushButton( "", m_capGroup );
	button->setPixmap( SmallIcon( "cap_square" ) );
	button->setToggleButton( true );
	m_capGroup->insert( button );
	mainLayout->addWidget( m_capGroup, 1, 1 );
	connect( m_capGroup, SIGNAL( clicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	
	QLabel* joinLabel = new QLabel( i18n ( "Join:" ), mainWidget );
	mainLayout->addWidget( joinLabel, 2, 0 );
	
	m_joinGroup = new QHButtonGroup( mainWidget );
	m_joinGroup->setFrameShape( NoFrame );
	m_joinGroup->setInsideMargin( 1 );
	m_joinGroup->setExclusive( true );
	button = new QPushButton( "", m_joinGroup );
	button->setPixmap( SmallIcon( "join_miter" ) );
	button->setToggleButton( true );
	m_joinGroup->insert( button );
	button = new QPushButton( "", m_joinGroup );
	button->setPixmap( SmallIcon( "join_round" ) );
	button->setToggleButton( true );
	m_joinGroup->insert( button );
	button = new QPushButton( "", m_joinGroup );
	button->setPixmap( SmallIcon( "join_bevel" ) );
	button->setToggleButton( true );
	m_joinGroup->insert( button );
	mainLayout->addWidget( m_joinGroup, 2, 1 );
	connect( m_joinGroup, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );

	mainLayout->activate();
	setWidget( mainWidget );

	updateDocker();
}

void VStrokeDocker::updateCanvas()
{
	if( m_part && m_part->document().selection()->objects().count() > 0 )
	{
		m_part->addCommand( new VStrokeCmd( &m_part->document(), &m_stroke ), true );
		m_view->selectionChanged();
	}
}

void VStrokeDocker::slotCapChanged( int ID )
{
	switch( ID )
	{
		case 1:
			m_stroke.setLineCap( VStroke::capRound ); break;
		case 2:
			m_stroke.setLineCap( VStroke::capSquare ); break;
		default:
			m_stroke.setLineCap( VStroke::capButt );
	}
	updateCanvas();
}

void VStrokeDocker::slotJoinChanged( int ID )
{
	switch( ID )
	{
		case 1:
			m_stroke.setLineJoin( VStroke::joinRound ); break;
		case 2:
			m_stroke.setLineJoin( VStroke::joinBevel ); break;
		default:
			m_stroke.setLineJoin( VStroke::joinMiter );
	}
	updateCanvas();
}

void VStrokeDocker::updateDocker()
{
	disconnect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	disconnect( m_capGroup, SIGNAL( clicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	disconnect( m_joinGroup, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );

	switch( m_stroke.lineCap() )
	{
		case VStroke::capRound:
			m_capGroup->setButton( 1 ); break;
		case VStroke::capSquare:
			m_capGroup->setButton( 2 ); break;
		default:
			m_capGroup->setButton( 0 );
	}

	switch( m_stroke.lineJoin() )
	{
		case VStroke::joinRound:
			m_joinGroup->setButton( 1 ); break;
		case VStroke::joinBevel:
			m_joinGroup->setButton( 2 ); break;
		default:
			m_joinGroup->setButton( 0 );
	}
	
	m_setLineWidth->setValue( m_stroke.lineWidth() );
	
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	connect( m_capGroup, SIGNAL( clicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	connect( m_joinGroup, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
}

void VStrokeDocker::widthChanged()
{
	m_stroke.setLineWidth( m_setLineWidth->value() );
	updateCanvas();
}

void VStrokeDocker::setStroke( const VStroke &stroke )
{
	m_stroke = stroke;
	updateDocker();
}

#include "vstrokedocker.moc"

