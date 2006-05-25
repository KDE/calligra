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

#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <qmatrix.h>
#include <QToolTip>
//Added by qt3to4:
#include <Q3GridLayout>

#include <klocale.h>
#include <KoMainWindow.h>
#include <KoRect.h>
#include <KoUnitWidgets.h>

#include "karbon_part.h"
#include "karbon_view.h"

#include "vselection.h"
#include "vtransformcmd.h"

#include "vtransformdocker.h"

VTransformDocker::VTransformDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: QWidget(), m_part ( part ), m_view( parent )
{
	setCaption( i18n( "Transform" ) );

	Q3GridLayout *mainLayout = new Q3GridLayout( this, 5, 5 );

	//X: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* xLabel = new QLabel( i18n ( "X:" ), this );
	mainLayout->addWidget( xLabel, 0, 0 );
	m_x = new KoUnitDoubleSpinBox( this, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_x, 0, 1 );
	m_x->setToolTip( i18n("Set x-position of actual selection") );

	//Y: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* yLabel = new QLabel( i18n ( "Y:" ), this );
	mainLayout->addWidget( yLabel, 0, 2 );
	m_y = new KoUnitDoubleSpinBox( this, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_y, 0, 3 );
	m_y->setToolTip( i18n("Set y-position of actual selection") );

	//Width: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* wLabel = new QLabel( i18n ( "W:" ), this );
	mainLayout->addWidget( wLabel, 1, 0 );
	m_width = new KoUnitDoubleSpinBox( this, 0.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_width, 1, 1 );
	m_width->setToolTip( i18n("Set width of actual selection") );

	//Height: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* hLabel = new QLabel( i18n ( "H:" ), this );
	mainLayout->addWidget( hLabel, 1, 2 );
	m_height = new KoUnitDoubleSpinBox( this, 0.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_height, 1, 3 );
	m_height->setToolTip( i18n("Set height of actual selection") );

	//TODO: Add Rotation, Shear
	//ROTATE: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* rLabel = new QLabel( i18n ( "R:" ), this );
	mainLayout->addWidget( rLabel, 3, 0 );
	/* port:
m_rotate = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, 1, this );
	mainLayout->addWidget( m_rotate, 3, 1 );
	m_rotate->setToolTip( i18n("Rotate actual selection") );
*/

	//X-Shear: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* sxLabel = new QLabel( i18n ( "SX:" ), this );
	mainLayout->addWidget( sxLabel, 2, 0 );
	m_shearX = new KoUnitDoubleSpinBox( this, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_shearX, 2, 1 );
	m_shearX->setToolTip( i18n("Shear actual selection in x-direction") );

	//Y-Shear: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* syLabel = new QLabel( i18n ( "SY:" ), this );
	mainLayout->addWidget( syLabel, 2, 2 );
	m_shearY = new KoUnitDoubleSpinBox( this, -5000.0, 5000.0, 1.0, 10.0, m_part->unit(), 1 );
	mainLayout->addWidget( m_shearY, 2, 3 );
	m_shearY->setToolTip( i18n("Shear actual selection in y-direction") );

	mainLayout->setRowStretch( 4, 1 );
	mainLayout->setColStretch( 1, 1 );
	mainLayout->setColStretch( 3, 1 );

	update();
}

void
VTransformDocker::enableSignals( bool enable )
{
	if( enable )
	{
		connect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		connect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		connect( m_width, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
		connect( m_height, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) ); 
		connect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		connect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		connect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
	}
	else
	{
		disconnect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		disconnect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		disconnect( m_width, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
		disconnect( m_height, SIGNAL( valueChanged( double ) ), this, SLOT( scale() ) );
		disconnect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		disconnect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		disconnect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
	}
}

void
VTransformDocker::update()
{
	enableSignals( false );

	int objcount = m_view->part()->document().selection()->objects().count();
	if ( objcount>0 )
	{
		setEnabled( true );
		KoRect rect = m_view->part()->document().selection()->boundingBox();

		m_x->changeValue( rect.x() );
		m_y->changeValue( rect.y() );
		m_width->changeValue( rect.width() );
		m_height->changeValue( rect.height() );
	}
	else
	{
		m_x->changeValue(0.0);
		m_y->changeValue(0.0);
		m_width->changeValue(0.0);
		m_height->changeValue(0.0);
		setEnabled( false );
	}

	m_shearX->changeValue(0.0);
	m_shearY->changeValue(0.0);
	m_rotate->setValue(0.0);

	enableSignals( true );
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

void 
VTransformDocker::setUnit( KoUnit::Unit unit )
{
	enableSignals( false );

	m_x->setUnit( unit );
	m_y->setUnit( unit );
	m_width->setUnit( unit );
	m_height->setUnit( unit );
	m_shearX->setUnit( unit );
	m_shearY->setUnit( unit );

	enableSignals( true );
}

void 
VTransformDocker::shearX()
{
	double shear = m_shearX->value();

	if( shear != 0.0 )
	{
		KoRect rect = m_view->part()->document().selection()->boundingBox();
		shear /= double(rect.width()*0.5);
		VShearCmd *cmd = new VShearCmd( &m_view->part()->document(), rect.center(), shear, 0 );
		m_view->part()->addCommand( cmd );
		m_part->repaintAllViews( true );
		disconnect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		m_shearX->changeValue(0.0);
		connect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
	}
}

void 
VTransformDocker::shearY()
{
	double shear = m_shearY->value();

	if( shear != 0.0 )
	{
		KoRect rect = m_view->part()->document().selection()->boundingBox();
		shear /= double(rect.height()*0.5);
		VShearCmd *cmd = new VShearCmd( &m_view->part()->document(), rect.center(), 0, shear );
		m_view->part()->addCommand( cmd );
		m_part->repaintAllViews( true );
		disconnect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		m_shearY->changeValue(0.0);
		connect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
	}
}

void
VTransformDocker::rotate()
{
	double angle = m_rotate->value();
	
	if( angle != 0.0 )
	{
		KoPoint center = m_view->part()->document().selection()->boundingBox().center();
		VRotateCmd *cmd = new VRotateCmd(  &m_view->part()->document(), center, angle );
		m_view->part()->addCommand( cmd );
		m_part->repaintAllViews( true );
		disconnect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
		m_rotate->setValue(0.0);
		connect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
	}
}

#include "vtransformdocker.moc"

