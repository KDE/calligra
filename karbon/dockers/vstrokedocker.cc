/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2005 The Karbon Developers
   Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>

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
#include <QPushButton>
#include <QWidget>
#include <QToolTip>
#include <QGridLayout>
#include <QDockWidget>
#include <QButtonGroup>

#include <kiconloader.h>
#include <klocale.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoDockFactory.h>
#include <KoUnitWidgets.h>
#include <KoShapeManager.h>
#include <KoCommand.h>
#include <KoShapeBorderModel.h>

#include "vstrokedocker.h"

VStrokeDockerFactory::VStrokeDockerFactory()
{
}

QString VStrokeDockerFactory::dockId() const
{
    return QString("Stroke Properties");
}

Qt::DockWidgetArea VStrokeDockerFactory::defaultDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

QDockWidget* VStrokeDockerFactory::createDockWidget()
{
    VStrokeDocker* widget = new VStrokeDocker();
    widget->setObjectName(dockId());

    return widget;
}


VStrokeDocker::VStrokeDocker()
{
	setWindowTitle( i18n( "Stroke Properties" ) );

	QWidget *mainWidget = new QWidget( this );
	QGridLayout *mainLayout = new QGridLayout( mainWidget );
	
	QLabel* widthLabel = new QLabel( i18n ( "Width:" ), mainWidget );
	mainLayout->addWidget( widthLabel, 0, 0 );
	// set min/max/step and value in points, then set actual unit
	m_setLineWidth = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 2 );
	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	m_setLineWidth->setUnit( canvasController->canvas()->unit() );
    m_setLineWidth->setToolTip( i18n( "Set line width of actual selection" ) );
	mainLayout->addWidget( m_setLineWidth, 0, 1, 1, 3 );
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) );
	
	QLabel* capLabel = new QLabel( i18n ( "Cap:" ), mainWidget );
	mainLayout->addWidget( capLabel, 1, 0 );
    m_capGroup = new QButtonGroup( mainWidget );
    m_capGroup->setExclusive( true );
	m_capGroup->setExclusive( true );

	QPushButton *button = 0;

	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "cap_butt" ) );
	button->setCheckable( true );
    button->setToolTip( i18n( "Butt cap" ) );
	m_capGroup->addButton( button, Qt::FlatCap );
	mainLayout->addWidget( button, 1, 1 );
	
	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "cap_round" ) );
	button->setCheckable( true );
    button->setToolTip( i18n( "Round cap" ) );
	m_capGroup->addButton( button, Qt::RoundCap );
	mainLayout->addWidget( button, 1, 2 );
	
	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "cap_square" ) );
	button->setCheckable( true );
    button->setToolTip( i18n( "Square cap" ) );
	m_capGroup->addButton( button, Qt::SquareCap );
	mainLayout->addWidget( button, 1, 3 );
	
	connect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	
	QLabel* joinLabel = new QLabel( i18n ( "Join:" ), mainWidget );
	mainLayout->addWidget( joinLabel, 2, 0 );
	
	m_joinGroup = new QButtonGroup( mainWidget );
	m_joinGroup->setExclusive( true );

	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "join_miter" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Miter join" ) );
	m_joinGroup->addButton( button, Qt::MiterJoin );
	mainLayout->addWidget( button, 2, 1 );
	
	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "join_round" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Round join" ) );
	m_joinGroup->addButton( button, Qt::RoundJoin );
	mainLayout->addWidget( button, 2, 2 );
	
	button = new QPushButton( mainWidget );
	button->setIcon( SmallIcon( "join_bevel" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Bevel join" ) );
	m_joinGroup->addButton( button, Qt::BevelJoin );
	mainLayout->addWidget( button, 2, 3 );
	
	connect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );

	QLabel* miterLabel = new QLabel( i18n ( "Miter limit:" ), mainWidget );
	mainLayout->addWidget( miterLabel, 3, 0 );
	// set min/max/step and value in points, then set actual unit
	m_miterLimit = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 2 );
	m_miterLimit->setUnit( canvasController->canvas()->unit() );
    m_miterLimit->setToolTip( i18n( "Set miter limit" ) );
	mainLayout->addWidget( m_miterLimit, 3, 1, 1, 3 );
	connect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );

	mainLayout->setRowStretch( 4, 1 );
	mainLayout->setColumnStretch( 1, 1 );
	mainLayout->setColumnStretch( 2, 1 );
	mainLayout->setColumnStretch( 3, 1 );

	setWidget( mainWidget );

	updateDocker();
}

void VStrokeDocker::updateCanvas()
{
	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
	if( ! selection )
		return;

	KoLineBorder *newBorder = new KoLineBorder();
	newBorder->setLineWidth( m_border.lineWidth() );
	newBorder->setCapStyle( m_border.capStyle() );
	newBorder->setJoinStyle( m_border.joinStyle() );
	newBorder->setMiterLimit( m_border.miterLimit() );

	KoShapeBorderCommand *cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
	canvasController->canvas()->addCommand( cmd, true );
}

void VStrokeDocker::slotCapChanged( int ID )
{
	m_border.setCapStyle( static_cast<Qt::PenCapStyle>( ID ) );
	updateCanvas();
}

void VStrokeDocker::slotJoinChanged( int ID )
{
	m_border.setJoinStyle( static_cast<Qt::PenJoinStyle>( ID ) );
	updateCanvas();
}

void VStrokeDocker::updateDocker()
{
	disconnect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	disconnect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	disconnect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	disconnect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );

	m_capGroup->button( m_border.capStyle() )->setChecked( true );
	m_joinGroup->button( m_border.joinStyle() )->setChecked( true );
	m_setLineWidth->changeValue( m_border.lineWidth() );
	m_miterLimit->changeValue( m_border.miterLimit() );
	
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	connect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	connect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	connect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );
}

void VStrokeDocker::widthChanged()
{
	m_border.setLineWidth( m_setLineWidth->value() );
	updateCanvas();
}

void VStrokeDocker::miterLimitChanged()
{
	m_border.setMiterLimit( m_miterLimit->value() );
	updateCanvas();
}

void VStrokeDocker::setStroke( const KoShapeBorderModel *border )
{
	const KoLineBorder *lineBorder = dynamic_cast<const KoLineBorder*>( border );
	if( lineBorder )
	{
		m_border.setLineWidth( lineBorder->lineWidth() );
		m_border.setCapStyle( lineBorder->capStyle() );
		m_border.setJoinStyle( lineBorder->joinStyle() );
		m_border.setMiterLimit( lineBorder->miterLimit() );
	}
	updateDocker();
}

void VStrokeDocker::setUnit( KoUnit::Unit unit )
{
	disconnect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	disconnect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	disconnect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	disconnect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );

	m_setLineWidth->setUnit( unit );

	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	connect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	connect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	connect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );
}
#include "vstrokedocker.moc"

