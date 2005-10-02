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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qbuttongroup.h>
#include <qnamespace.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <klocale.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <koMainWindow.h>
#include <kseparator.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "vtool.h"
#include "vtoolcontroller.h"
#include "vtypebuttonbox.h"

#include "vtoolbox.h"
#include "vstrokefillpreview.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

VToolBox::VToolBox( KarbonPart* part, KMainWindow *mainWin, const char* name ) : KToolBar( mainWin, name, false, false ), m_part( part )
{
	setFullSize( false );
	buttonGroup = new QButtonGroup( 0L );
	buttonGroup->setExclusive( true );
	connect( buttonGroup, SIGNAL( pressed( int ) ), this, SLOT( slotButtonPressed( int ) ) );

	QBoxLayout::Direction d = orientation() == Qt::Vertical ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	QWidget *base = new QWidget( this );
	columnsLayouter = new QBoxLayout( base, d );
	//columnsLayouter->setSpacing( 2 );

	d = orientation() == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	left = new QWidget( base );
	leftLayout = new QBoxLayout( left, d );
	//leftLayout->setSpacing( 2 );
	columnsLayouter->addWidget( left );

	right = new QWidget( base );
	rightLayout = new QBoxLayout( right, d );
	//rightLayout->setSpacing( 2 );
	columnsLayouter->addWidget( right );

	insertLeft = true;

	m_strokeFillPreview = new VStrokeFillPreview( part, this );
	m_typeButtonBox = new VTypeButtonBox( part, this );

	connect( m_strokeFillPreview, SIGNAL( fillSelected() ),
		m_typeButtonBox, SLOT( setFill() ) );
	connect( m_strokeFillPreview, SIGNAL( strokeSelected() ),
		m_typeButtonBox, SLOT( setStroke() ) );

	// setup tool collections
	m_manipulationtools.resize( 10 );
	m_shapetools.resize( 20 );
	m_misctools.resize( 30 );
}

void
VToolBox::slotPressButton( int id )
{
	buttonGroup->setButton( id );
	slotButtonPressed( id );
}

void
VToolBox::slotButtonPressed( int id )
{
	if( id != buttonGroup->selectedId() && buttonGroup->selected() ) buttonGroup->selected()->setDown( false );
	int shapestart = m_manipulationtools.count();
	int miscstart = m_manipulationtools.count() + m_shapetools.count();
	int miscend = miscstart + m_misctools.count();
	if( id < shapestart )
		emit activeToolChanged( m_manipulationtools.at( id ) );
	else if( id < miscstart )
		emit activeToolChanged( m_shapetools.at( id - shapestart ) );
	else if( id < miscend )
		emit activeToolChanged( m_misctools.at( id - miscstart ) );
}

void
VToolBox::registerTool( VTool *tool )
{
	kdDebug(38000) << "VToolBox::registerTool : " << tool->name() << endl;
	uint prio = tool->priority();
	if( tool->category() == "shapecreation" )
		m_shapetools.insert( ( prio == 0 ) ? m_shapetools.count() : prio - 1, tool );
	else if( tool->category() == "manipulation" )
		m_manipulationtools.insert( ( prio == 0 ) ? m_manipulationtools.count() : prio - 1, tool );
	else
		m_misctools.insert( ( prio == 0 ) ? m_misctools.count() : prio - 1, tool );
}

void
VToolBox::setupTools()
{
	QDictIterator<VTool> itr( m_part->toolController()->tools() );
	kdDebug(38000) << "count : " << m_part->toolController()->tools().count() << endl;
	for( ; itr.current() ; ++itr )
		registerTool( itr.current() );

	int id = 0;
	for( uint i = 0; i < m_manipulationtools.count() ; i++ )
	{
		VTool *tool = m_manipulationtools.at( i );
		if( tool )
			addButton( tool->icon().latin1(), tool->name(), id++ );
	}

	for( uint i = 0; i < m_shapetools.count() ; i++ )
	{
		VTool *tool = m_shapetools.at( i );
		if( tool )
			addButton( tool->icon().latin1(), tool->name(), id++ );
	}

	for( uint i = 0; i < m_misctools.count() ; i++ )
	{
		VTool *tool = m_misctools.at( i );
		if( tool )
			addButton( tool->icon().latin1(), tool->name(), id++ );
	}
	if( !insertLeft ) // uneven count, make dummy button
		addButton( "karbon", "", id );
	// select first (select tool)
	buttonGroup->setButton( 0 );
}

QToolButton *
VToolBox::addButton( const char* iconName, QString tooltip, int id )
{
	kdDebug(38000) << "Adding : " << iconName << endl;
	QToolButton *button = new QToolButton( insertLeft ? left : right );
	if( iconName != "" )
	{
		QPixmap pixmap = BarIcon( iconName, KarbonFactory::instance() );
		button->setPixmap( pixmap );
		button->setToggleButton( true );
	}
	if( !tooltip.isEmpty() )
	QToolTip::add( button, tooltip );
	if( insertLeft )
		leftLayout->addWidget( button );
	else
		rightLayout->addWidget( button );

	buttonGroup->insert( button, id );
	insertLeft =! insertLeft;

	return button;
}


void
VToolBox::setOrientation ( Qt::Orientation o )
{
	if( barPos() == Floating ) { // when floating, make it a standing toolbox.
		o = o == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
	}
	QBoxLayout::Direction d = o == Qt::Vertical ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	columnsLayouter->setDirection( d );
	d = o == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	leftLayout->setDirection( d );
	rightLayout->setDirection( d );
	QDockWindow::setOrientation( o );
}

VStrokeFillPreview *
VToolBox::strokeFillPreview()
{
	return m_strokeFillPreview;
}

#include "vtoolbox.moc"

