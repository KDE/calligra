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

#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <klocale.h>
#include <kiconloader.h>
#include <koMainWindow.h>
#include <kseparator.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"

#include "vtooldocker.h"

VToolDocker::VToolDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setOrientation( Vertical );
	setCloseMode( QDockWindow::Never );
	QToolButton *button;

	m_btngroup = new QButtonGroup( 2, Horizontal, this );
	m_btngroup->setExclusive( true );
	m_btngroup->setInsideSpacing( 2 );
	m_btngroup->setInsideMargin( 5 );

	button = new QToolButton( m_btngroup );
	QPixmap pixmap = BarIcon( "14_select", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	button->toggle();
	connect( button, SIGNAL( clicked() ), this, SIGNAL( selectToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_selectnodes", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( selectNodesToolActivated() ) );
	m_btngroup->insert( button );

    button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_rotate", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rotateToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_shear", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( shearToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_rectangle", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rectangleToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_roundrect", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( roundRectToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_ellipse", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( ellipseToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap=BarIcon( "14_polygon", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( polygonToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_star", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( starToolActivated() ) );
	m_btngroup->insert( button );

    button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_sinus", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( sinusToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_spiral", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( spiralToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap = BarIcon( "14_gradient", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( gradToolActivated() ) );
	m_btngroup->insert( button );

	button = new QToolButton( m_btngroup );
	pixmap=BarIcon( "14_polyline", KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( polylineToolActivated() ) );
	m_btngroup->insert( button );

	setWidget( m_btngroup );
}

#include "vtooldocker.moc"

