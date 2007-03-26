/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <t.zachmann@zagge.de>

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
#include <QRadioButton>
#include <QWidget>
#include <QToolTip>
#include <QGridLayout>
#include <QDockWidget>
#include <QButtonGroup>

#include <kiconloader.h>
#include <klocale.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoDockFactory.h>
#include <KoUnitWidgets.h>
#include <KoShapeManager.h>
#include <KoShapeBorderCommand.h>
#include <KoShapeBorderModel.h>
#include <KoSelection.h>

#include "KarbonLineStyleSelector.h"

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
	m_setLineWidth = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit(KoUnit::Point), 2 );
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

	QRadioButton *button = 0;

	button = new QRadioButton( mainWidget );
	button->setIcon( SmallIcon( "cap_butt" ) );
	button->setCheckable( true );
    button->setToolTip( i18n( "Butt cap" ) );
	m_capGroup->addButton( button, Qt::FlatCap );
	mainLayout->addWidget( button, 1, 1 );
	
	button = new QRadioButton( mainWidget );
	button->setIcon( SmallIcon( "cap_round" ) );
	button->setCheckable( true );
    button->setToolTip( i18n( "Round cap" ) );
	m_capGroup->addButton( button, Qt::RoundCap );
	mainLayout->addWidget( button, 1, 2 );
	
	button = new QRadioButton( mainWidget );
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

	button = new QRadioButton( mainWidget );
	button->setIcon( SmallIcon( "join_miter" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Miter join" ) );
	m_joinGroup->addButton( button, Qt::MiterJoin );
	mainLayout->addWidget( button, 2, 1 );
	
	button = new QRadioButton( mainWidget );
	button->setIcon( SmallIcon( "join_round" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Round join" ) );
	m_joinGroup->addButton( button, Qt::RoundJoin );
	mainLayout->addWidget( button, 2, 2 );
	
	button = new QRadioButton( mainWidget );
	button->setIcon( SmallIcon( "join_bevel" ) );
	button->setCheckable( true );
	button->setToolTip( i18n( "Bevel join" ) );
	m_joinGroup->addButton( button, Qt::BevelJoin );
	mainLayout->addWidget( button, 2, 3 );
	
	connect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );

	QLabel* miterLabel = new QLabel( i18n ( "Miter limit:" ), mainWidget );
	mainLayout->addWidget( miterLabel, 3, 0 );
	// set min/max/step and value in points, then set actual unit
	m_miterLimit = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit(KoUnit::Point), 2 );
	m_miterLimit->setUnit( canvasController->canvas()->unit() );
    m_miterLimit->setToolTip( i18n( "Set miter limit" ) );
	mainLayout->addWidget( m_miterLimit, 3, 1, 1, 3 );
	connect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );

    QLabel * styleLabel = new QLabel( i18n( "Style:" ), mainWidget );
    mainLayout->addWidget( styleLabel, 4, 0 );
    m_lineStyle = new KarbonLineStyleSelector( mainWidget );
    mainLayout->addWidget( m_lineStyle, 4, 1, 1, 3 );

    connect( m_lineStyle, SIGNAL(currentIndexChanged( int ) ), this, SLOT( styleChanged() ) );

    mainLayout->setRowStretch( 5, 1 );
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
    if( ! selection || ! selection->count() )
        return;

    KoLineBorder * newBorder = new KoLineBorder();
    newBorder->setLineWidth( m_border.lineWidth() );
    newBorder->setCapStyle( m_border.capStyle() );
    newBorder->setJoinStyle( m_border.joinStyle() );
    newBorder->setMiterLimit( m_border.miterLimit() );
    newBorder->setLineStyle( m_border.lineStyle(), m_border.lineDashes() );
    KoLineBorder * oldBorder = dynamic_cast<KoLineBorder*>( selection->firstSelectedShape()->border() );
    if( oldBorder )
        newBorder->setColor( oldBorder->color() );

    KoShapeBorderCommand *cmd = new KoShapeBorderCommand( selection->selectedShapes(), newBorder );
    canvasController->canvas()->addCommand( cmd );
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
    disconnect( m_lineStyle, SIGNAL(currentIndexChanged( int ) ), this, SLOT( styleChanged() ) );

	m_capGroup->button( m_border.capStyle() )->setChecked( true );
	m_joinGroup->button( m_border.joinStyle() )->setChecked( true );
	m_setLineWidth->changeValue( m_border.lineWidth() );
	m_miterLimit->changeValue( m_border.miterLimit() );
    m_lineStyle->setLineStyle( m_border.lineStyle(), m_border.lineDashes() );
	
	connect( m_setLineWidth, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged() ) ); 
	connect( m_capGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	connect( m_joinGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	connect( m_miterLimit, SIGNAL( valueChanged( double ) ), this, SLOT( miterLimitChanged() ) );
    connect( m_lineStyle, SIGNAL(currentIndexChanged( int ) ), this, SLOT( styleChanged() ) );
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

void VStrokeDocker::styleChanged()
{
    m_border.setLineStyle( m_lineStyle->lineStyle(), m_lineStyle->lineDashes() );
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
        m_border.setLineStyle( lineBorder->lineStyle(), lineBorder->lineDashes() );
	}
	else
	{
		m_border.setLineWidth( 0.0 );
		m_border.setCapStyle( Qt::FlatCap );
		m_border.setJoinStyle( Qt::MiterJoin );
		m_border.setMiterLimit( 0.0 );
        m_border.setLineStyle( Qt::NoPen, QVector<qreal>() );
	}
	updateDocker();
}

void VStrokeDocker::setUnit( KoUnit unit )
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

