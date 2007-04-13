/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2003 Stephan Binner <binner@kde.org>
   Copyright (C) 2004-2005 David Faure <faure@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2006 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
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
#include <QLayout>
#include <QWidget>
#include <QDockWidget>
#include <QMatrix>
#include <QToolTip>
#include <QGridLayout>
#include <QPointF>
#include <QRectF>

#include <klocale.h>
#include <KoMainWindow.h>
#include <KoDockFactory.h>
#include <KoUnitWidgets.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeMoveCommand.h>
#include <KoShapeRotateCommand.h>
#include <KoShapeShearCommand.h>
#include <KoShapeSizeCommand.h>
#include <KoShapeContainer.h>

#include <kdebug.h>

#include "vtransformdocker.h"

VTransformDockerFactory::VTransformDockerFactory()
{
}

QString VTransformDockerFactory::id() const
{
    return QString("Transform");
}

Qt::DockWidgetArea VTransformDockerFactory::defaultDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

QDockWidget* VTransformDockerFactory::createDockWidget()
{
    VTransformDocker* widget = new VTransformDocker();
    widget->setObjectName(id());

    return widget;
}

VTransformDocker::VTransformDocker()
{
	setWindowTitle( i18n( "Transform" ) );

	QWidget *mainWidget = new QWidget( this );
	QGridLayout *mainLayout = new QGridLayout( mainWidget );
	
	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	KoUnit documentUnit = canvasController->canvas()->unit();
	//X: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* xLabel = new QLabel( i18n ( "X:" ), mainWidget );
	mainLayout->addWidget( xLabel, 0, 0 );
	m_x = new KoUnitDoubleSpinBox( mainWidget, -5000.0, 5000.0, 1.0, 10.0, documentUnit, 1 );
	mainLayout->addWidget( m_x, 0, 1 );
	m_x->setToolTip( i18n("Set x-position of actual selection") );

	//Y: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* yLabel = new QLabel( i18n ( "Y:" ), mainWidget );
	mainLayout->addWidget( yLabel, 0, 2 );
	m_y = new KoUnitDoubleSpinBox( mainWidget, -5000.0, 5000.0, 1.0, 10.0, documentUnit, 1 );
	mainLayout->addWidget( m_y, 0, 3 );
	m_y->setToolTip( i18n("Set y-position of actual selection") );

	//Width: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* wLabel = new QLabel( i18n ( "W:" ), mainWidget );
	mainLayout->addWidget( wLabel, 1, 0 );
	m_width = new KoUnitDoubleSpinBox( mainWidget, 0.0, 5000.0, 1.0, 10.0, documentUnit, 1 );
	mainLayout->addWidget( m_width, 1, 1 );
	m_width->setToolTip( i18n("Set width of actual selection") );

	//Height: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* hLabel = new QLabel( i18n ( "H:" ), mainWidget );
	mainLayout->addWidget( hLabel, 1, 2 );
	m_height = new KoUnitDoubleSpinBox( mainWidget, 0.0, 5000.0, 1.0, 10.0, documentUnit, 1 );
	mainLayout->addWidget( m_height, 1, 3 );
	m_height->setToolTip( i18n("Set height of actual selection") );

	//ROTATE: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* rLabel = new QLabel( i18n ( "R:" ), mainWidget );
	mainLayout->addWidget( rLabel, 3, 0 );
	m_rotate = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
	mainLayout->addWidget( m_rotate, 3, 1 );
	m_rotate->setToolTip( i18n("Rotate actual selection") );

	//X-Shear: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* sxLabel = new QLabel( i18n ( "SX:" ), mainWidget );
	mainLayout->addWidget( sxLabel, 2, 0 );
	m_shearX = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
	mainLayout->addWidget( m_shearX, 2, 1 );
	m_shearX->setToolTip( i18n("Shear actual selection in x-direction") );

	//Y-Shear: (TODO: Set 5000 limit to real Karbon14 limit)
	QLabel* syLabel = new QLabel( i18n ( "SY:" ), mainWidget );
	mainLayout->addWidget( syLabel, 2, 2 );
	m_shearY = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
	mainLayout->addWidget( m_shearY, 2, 3 );
	m_shearY->setToolTip( i18n("Shear actual selection in y-direction") );

	mainLayout->setRowStretch( 4, 1 );
	mainLayout->setColumnStretch( 1, 1 );
	mainLayout->setColumnStretch( 3, 1 );

	setWidget( mainWidget );

	update();
}

void
VTransformDocker::enableSignals( bool enable )
{
	if( enable )
	{
		connect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		connect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		connect( m_width, SIGNAL( valueChangedPt( double ) ), this, SLOT( scale() ) );
		connect( m_height, SIGNAL( valueChangedPt( double ) ), this, SLOT( scale() ) );
		connect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		connect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		connect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
	}
	else
	{
		disconnect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		disconnect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( translate() ) );
		disconnect( m_width, SIGNAL( valueChangedPt( double ) ), this, SLOT( scale() ) );
		disconnect( m_height, SIGNAL( valueChangedPt( double ) ), this, SLOT( scale() ) );
		disconnect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		disconnect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		disconnect( m_rotate, SIGNAL( valueChanged( double ) ), this, SLOT( rotate() ) );
	}
}

void
VTransformDocker::update()
{
	enableSignals( false );

	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
	int objcount = selection->count();
	if ( objcount>0 )
	{
		setEnabled( true );
		QRectF rect = selection->boundingRect();

		m_x->changeValue( rect.x() );
		m_y->changeValue( rect.y() );
		m_width->changeValue( rect.width() );
		m_height->changeValue( rect.height() );

		m_shearX->setValue( selection->shearX() );
		m_shearY->setValue( selection->shearY() );
		m_rotate->setValue( selection->rotation() );
	}
	else
	{
		m_x->changeValue(0.0);
		m_y->changeValue(0.0);
		m_width->changeValue(0.0);
		m_height->changeValue(0.0);
		m_shearX->setValue(0.0);
		m_shearY->setValue(0.0);
		m_rotate->setValue(0.0);
		setEnabled( false );
	}

	enableSignals( true );
}

void
VTransformDocker::translate()
{
    QPointF newPos( m_x->value(), m_y->value() );

    kDebug(38000) << "translating to: " << newPos << endl;

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    QRectF rect = selection->boundingRect();

    if( rect.x() != newPos.x() || rect.y() != newPos.y() )
    {
        QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
        QPointF moveBy = newPos - selection->position();
        QList<QPointF> oldPositions;
        QList<QPointF> newPositions;
        foreach( KoShape* shape, selectedShapes )
        {
            oldPositions.append( shape->position() );
            newPositions.append( shape->position() + moveBy );
        }
        canvasController->canvas()->addCommand( new KoShapeMoveCommand( selectedShapes, oldPositions, newPositions ) );
    }
    update();
}

void
VTransformDocker::scale()
{
	QSizeF newSize( m_width->value(), m_height->value() );

	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
	QRectF rect = selection->boundingRect();

	if( rect.width() != newSize.width() || rect.height() != newSize.height() )
	{
		QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
		QList<QSizeF> oldSizes, newSizes;
		foreach( KoShape* shape, selectedShapes )
		{
			kDebug(38000) << "scale from: " << shape->size() << " to: " << newSize << endl;
			oldSizes << shape->size();
			newSizes << newSize;
		}
		//canvasController->canvas()->addCommand( new KoShapeSizeCommand( selectedShapes, oldSizes, newSizes ), true );
	}
}

void 
VTransformDocker::setUnit( KoUnit unit )
{
	enableSignals( false );

	m_x->setUnit( unit );
	m_y->setUnit( unit );
	m_width->setUnit( unit );
	m_height->setUnit( unit );

	enableSignals( true );
}

void 
VTransformDocker::shearX()
{
	double shear = m_shearX->value();

	if( shear != 0.0 )
	{
		/*
		QRectF rect = m_view->part()->document().selection()->boundingBox();
		shear /= double(rect.width()*0.5);
		VShearCmd *cmd = new VShearCmd( &m_view->part()->document(), rect.center(), shear, 0 );
		 KoToolManager::instance()->activeCanvasController()->addCommand( cmd );
		m_part->repaintAllViews( true );
		disconnect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		m_shearX->changeValue(0.0);
		connect( m_shearX, SIGNAL( valueChanged( double ) ), this, SLOT( shearX() ) );
		*/
	}
}

void 
VTransformDocker::shearY()
{
	double shear = m_shearY->value();

	if( shear != 0.0 )
	{
		/*
		QRectF rect = m_view->part()->document().selection()->boundingBox();
		shear /= double(rect.height()*0.5);
		VShearCmd *cmd = new VShearCmd( &m_view->part()->document(), rect.center(), 0, shear );
		 KoToolManager::instance()->activeCanvasController()->canvas()->addCommand( cmd );
		m_part->repaintAllViews( true );
		disconnect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		m_shearY->changeValue(0.0);
		connect( m_shearY, SIGNAL( valueChanged( double ) ), this, SLOT( shearY() ) );
		*/
	}
}

void
VTransformDocker::rotate()
{
	double angle = m_rotate->value();
	
	KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
	KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
	QList<KoShape*> selectedShapes = selection->selectedShapes();

	QPointF center = selection->boundingRect().center();
	double initialAngle = selection->rotation();
	double relativeAngle = angle - initialAngle;

	QMatrix matrix;
	matrix.translate( center.x(), center.y() );
	matrix.rotate( relativeAngle );
	matrix.translate( -center.x(), -center.y() );

	QList<double> oldAngles, newAngles;
	QList<QPointF> oldPositions, newPositions;

	foreach( KoShape* shape, selectedShapes )
	{
		oldAngles << shape->rotation();
		oldPositions << shape->position();
		shape->setAbsolutePosition( matrix.map( shape->absolutePosition() ) );
		newAngles << shape->rotation() + relativeAngle;
		newPositions << shape->position();
	}
	selection->rotate( selection->rotation() + relativeAngle );
	QUndoCommand *cmd = new QUndoCommand("Rotate");
	new KoShapeMoveCommand( selectedShapes, oldPositions, newPositions, cmd );
	new KoShapeRotateCommand( selectedShapes, oldAngles, newAngles, cmd );
	canvasController->canvas()->addCommand( cmd );
}

#include "vtransformdocker.moc"

