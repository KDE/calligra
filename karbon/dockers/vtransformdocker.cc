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

#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qwmatrix.h>

#include <klocale.h>
#include <koMainWindow.h>
#include <koRect.h>
#include <koUnitWidgets.h>

#include "karbon_part.h"
#include "karbon_view.h"

#include "vselection.h"
#include "vtransformcmd.h"

#include "vtransformdocker.h"

VTransformDocker::VTransformDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: VDocker( parent->shell() ), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Transform" ) );
	setCloseMode( QDockWindow::Always );

	mainWidget = new QWidget( this );
	QGridLayout *mainLayout = new QGridLayout( mainWidget, 4, 4 );

	//X: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* xLabel = new QLabel( i18n ( "X:" ), mainWidget );
	mainLayout->addWidget( xLabel, 1, 0 );
	m_x = new KoUnitDoubleSpinBox( mainWidget, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_x, 1, 1 );

	//Y: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* yLabel = new QLabel( i18n ( "Y:" ), mainWidget );
	mainLayout->addWidget( yLabel, 2, 0 );
	m_y = new KoUnitDoubleSpinBox( mainWidget, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_y, 2, 1 );

	//Width: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* wLabel = new QLabel( i18n ( "W:" ), mainWidget );
	mainLayout->addWidget( wLabel, 1, 2 );
	m_width = new KoUnitDoubleSpinBox( mainWidget, 0.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_width, 1, 3 );

	//Height: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* hLabel = new QLabel( i18n ( "H:" ), mainWidget );
	mainLayout->addWidget( hLabel, 2, 2 );
	m_height = new KoUnitDoubleSpinBox( mainWidget, 0.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_height, 2, 3 );

	//TODO: Add Rotation, Shear

	setWidget( mainWidget );
	update();
}

void
VTransformDocker::update()
{
	disconnect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
	disconnect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
	disconnect( m_width, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
	disconnect( m_height, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );

	// update units in case they changed
	m_x->setUnit( m_view->part()->unit() );
	m_y->setUnit( m_view->part()->unit() );
	m_width->setUnit( m_view->part()->unit() );
	m_height->setUnit( m_view->part()->unit() );

	int objcount = m_view->part()->document().selection()->objects().count();
	if ( objcount>0 )
	{
		mainWidget->setEnabled( true );
		KoRect rect = m_view->part()->document().selection()->boundingBox();

		m_x->setValue( KoUnit::toUserValue( rect.x(), m_view->part()->unit() ) );
		m_y->setValue( KoUnit::toUserValue( rect.y(), m_view->part()->unit() ) );
		m_width->setValue( KoUnit::toUserValue( rect.width(), m_view->part()->unit() ) );
		m_height->setValue( KoUnit::toUserValue( rect.height(), m_view->part()->unit() ) );
	}
	else
	{
		m_x->setValue(0.0);
		m_y->setValue(0.0);
		m_width->setValue(0.0);
		m_height->setValue(0.0);
		mainWidget->setEnabled( false );
	}

	connect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
	connect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
	connect( m_width, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
	connect( m_height, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
}

void
VTransformDocker::translate()
{
	//FIXME: Needs an appropriate transform command which takes absolute values of object size
	double newX = m_x->value();
	double newY = m_y->value();

	KoRect rect = m_view->part()->document().selection()->boundingBox();

	if( rect.x() != newX || rect.y() != newY )
	{
		VTranslateCmd *cmd = new VTranslateCmd( &m_view->part()->document(), newX-rect.x(), newY-rect.y(), false );
		m_view->part()->addCommand( cmd );
	}
	m_part->repaintAllViews( true );
}

void
VTransformDocker::scale()
{
	//FIXME: Needs an appropriate transform command which takes absolute values of object size
	double newW = m_width->value();
	double newH = m_height->value();

	KoRect rect = m_view->part()->document().selection()->boundingBox();

	if( rect.width() != newW || rect.height() != newH )
	{
		
		VScaleCmd *cmd = new VScaleCmd( &m_view->part()->document(), rect.topLeft(), newW/rect.width(), newH/rect.height(), false );
		m_view->part()->addCommand( cmd );
	}
	m_part->repaintAllViews( true );
}

#include "vtransformdocker.moc"

