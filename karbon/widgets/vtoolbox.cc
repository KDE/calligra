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
#include <qnamespace.h>
#include <qtoolbutton.h>
#include <qlayout.h>
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

#include "vtoolbox.h"
#include "vstrokefillpreview.h"

VToolBox::VToolBox( KarbonPart* part, KMainWindow *mainWin, const char* name ) : KToolBar( mainWin, name, false, false )
{
	setFullSize( false );
	buttonGroup = new QButtonGroup( 0L );
	buttonGroup->setExclusive( true );

	QBoxLayout::Direction d = orientation() == Qt::Vertical ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	QWidget *base = new QWidget( this );
	columnsLayouter = new QBoxLayout( base, d );
	columnsLayouter->setSpacing( 2 );

	d = orientation() == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
	left= new QWidget( base );
	leftLayout = new QBoxLayout( left, d );
	leftLayout->setSpacing( 2 );
	columnsLayouter->addWidget( left );

	right= new QWidget( base );
	rightLayout = new QBoxLayout( right, d );
	rightLayout->setSpacing( 2 );
	columnsLayouter->addWidget( right );

	insertLeft = true;
	QToolButton *button = addButton( "14_select", i18n( "Select tool" ) );
	button->toggle();
	connect( button, SIGNAL( clicked() ), this, SIGNAL( selectToolActivated() ) );

	button = addButton( "14_selectnodes", i18n( "Select nodes tool") );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( selectNodesToolActivated() ) );

	button = addButton( "14_rotate", i18n( "rotate tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rotateToolActivated() ) );

	button = addButton("14_shear", i18n( "shear tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( shearToolActivated() ) );

	button = addButton( "14_rectangle", i18n( "create rectangle tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( rectangleToolActivated() ) );

	button = addButton( "14_roundrect", i18n( "create rounded rectangle tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( roundRectToolActivated() ) );

	button = addButton( "14_ellipse", i18n( "create ellipse tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( ellipseToolActivated() ) );

	button = addButton( "14_polygon", i18n( "create poligon tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( polygonToolActivated() ) );

	button = addButton( "14_star", i18n( "create star tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( starToolActivated() ) );

	button = addButton( "14_sinus", i18n("create sinus wave tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( sinusToolActivated() ) );

	button = addButton( "14_spiral", i18n( "create spiral tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( spiralToolActivated() ) );

	button = addButton( "14_gradient", i18n( "fill with gradient tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( gradToolActivated() ) );

	button = addButton( "14_polyline", i18n( "create polyline tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( polylineToolActivated() ) );

	button = addButton( "14_clipart", i18n( "insert clipart tool" ) );
	connect( button, SIGNAL( clicked() ), this, SIGNAL( clipartToolActivated() ) );

	m_strokeFillPreview = new VStrokeFillPreview( part, this );
}

QToolButton *
VToolBox::addButton( const char* iconName, QString tooltip )
{
	QToolButton *button = new QToolButton( insertLeft?left:right );
	QPixmap pixmap=BarIcon( iconName, KarbonFactory::instance() );
	button->setPixmap( pixmap );
	button->setToggleButton( true );
	QToolTip::add( button, tooltip );
	if( insertLeft )
		leftLayout->addWidget( button );
	else
		rightLayout->addWidget( button );

	buttonGroup->insert( button );
	insertLeft =! insertLeft;

	return button;
}


void
VToolBox::setOrientation ( Qt::Orientation o )
{
	if( barPos() == Floating ) { // when floating, make it a standing toolbox.
		o = o==Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
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

