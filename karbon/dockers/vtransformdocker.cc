/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002-2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2003 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2003 Stephan Binner <binner@kde.org>
   Copyright (C) 2004-2005 David Faure <faure@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
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

#include "vtransformdocker.h"

#include <KoDockFactory.h>
#include <KoUnitDoubleSpinBox.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeMoveCommand.h>
#include <KoShapeSizeCommand.h>
#include <KoShapeTransformCommand.h>

#include <klocale.h>
#include <knuminput.h>

#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QMatrix>
#include <QtGui/QToolTip>
#include <QtGui/QGridLayout>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

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

    //TODO: Set 5000 limit to real Karbon14 limit
    const double limit = 5000.0;

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoUnit documentUnit = canvasController->canvas()->unit();

    //X:
    QLabel* xLabel = new QLabel( i18n ( "X:" ), mainWidget );
    mainLayout->addWidget( xLabel, 0, 0 );
    m_x = new KoUnitDoubleSpinBox( mainWidget );
    m_x->setMinMaxStep( -limit, limit, 1.0 );
    m_x->setUnit( documentUnit );
    m_x->setDecimals( 1 );
    m_x->setToolTip( i18n("Set x-position of actual selection") );
    mainLayout->addWidget( m_x, 0, 1 );
    connect( m_x, SIGNAL( editingFinished() ), this, SLOT( translate() ) );

    //Y:
    QLabel* yLabel = new QLabel( i18n ( "Y:" ), mainWidget );
    mainLayout->addWidget( yLabel, 0, 2 );
    m_y = new KoUnitDoubleSpinBox( mainWidget );
    m_y->setMinMaxStep( -limit, limit, 1.0 );
    m_y->setUnit( documentUnit );
    m_y->setDecimals( 1 );
    m_y->setToolTip( i18n("Set y-position of actual selection") );
    mainLayout->addWidget( m_y, 0, 3 );
    connect( m_y, SIGNAL( editingFinished() ), this, SLOT( translate() ) );

    //Width:
    QLabel* wLabel = new QLabel( i18n ( "W:" ), mainWidget );
    mainLayout->addWidget( wLabel, 1, 0 );
    m_width = new KoUnitDoubleSpinBox( mainWidget );
    m_width->setMinMaxStep( 0.0, limit, 1.0 );
    m_width->setUnit( documentUnit );
    m_width->setDecimals( 1 );
    m_width->setToolTip( i18n("Set width of actual selection") );
    mainLayout->addWidget( m_width, 1, 1 );
    connect( m_width, SIGNAL( editingFinished() ), this, SLOT( scale() ) );

    //Height:
    QLabel* hLabel = new QLabel( i18n ( "H:" ), mainWidget );
    mainLayout->addWidget( hLabel, 1, 2 );
    m_height = new KoUnitDoubleSpinBox( mainWidget );
    m_height->setMinMaxStep( 0.0, limit, 1.0 );
    m_height->setUnit( documentUnit );
    m_height->setDecimals( 1 );
    m_height->setToolTip( i18n("Set height of actual selection") );
    mainLayout->addWidget( m_height, 1, 3 );
    connect( m_height, SIGNAL( editingFinished() ), this, SLOT( scale() ) );

    //ROTATE:
    QLabel* rLabel = new QLabel( i18n ( "R:" ), mainWidget );
    mainLayout->addWidget( rLabel, 3, 0 );
    m_rotate = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
    mainLayout->addWidget( m_rotate, 3, 1 );
    m_rotate->setToolTip( i18n("Rotate actual selection") );
    connect( m_rotate, SIGNAL( editingFinished() ), this, SLOT( rotate() ) );

    //X-Shear:
    QLabel* sxLabel = new QLabel( i18n ( "SX:" ), mainWidget );
    mainLayout->addWidget( sxLabel, 2, 0 );
    m_shearX = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
    mainLayout->addWidget( m_shearX, 2, 1 );
    m_shearX->setToolTip( i18n("Shear actual selection in x-direction") );
    connect( m_shearX, SIGNAL( editingFinished() ), this, SLOT( shear() ) );

    //Y-Shear:
    QLabel* syLabel = new QLabel( i18n ( "SY:" ), mainWidget );
    mainLayout->addWidget( syLabel, 2, 2 );
    m_shearY = new KDoubleSpinBox( -360.0, 360.0, 1.0, 10.0, mainWidget, 1 );
    mainLayout->addWidget( m_shearY, 2, 3 );
    m_shearY->setToolTip( i18n("Shear actual selection in y-direction") );
    connect( m_shearY, SIGNAL( editingFinished() ), this, SLOT( shear() ) );

    mainLayout->setRowStretch( 4, 1 );
    mainLayout->setColumnStretch( 1, 1 );
    mainLayout->setColumnStretch( 3, 1 );

    setWidget( mainWidget );

    update();
}

void VTransformDocker::enableSignals( bool enable )
{
    m_x->blockSignals( ! enable );
    m_y->blockSignals( ! enable );
    m_width->blockSignals( ! enable );
    m_height->blockSignals( ! enable );
    m_shearX->blockSignals( ! enable );
    m_shearY->blockSignals( ! enable );
    m_rotate->blockSignals( ! enable );
}

QRectF VTransformDocker::selectionRect()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    QRectF aaBB;
    QList<KoShape*> selectedShapes = selection->selectedShapes();
    foreach( KoShape * shape, selectedShapes )
    {
        if( aaBB.isEmpty() )
            aaBB = shape->transformationMatrix(0).map( shape->outline() ).boundingRect();
        else
            aaBB = aaBB.united( shape->transformationMatrix(0).map( shape->outline() ).boundingRect() );
    }
   return aaBB;
}

void VTransformDocker::update()
{
    enableSignals( false );

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if ( selection->count() > 0 )
    {
        setEnabled( true );
        QRectF rect = selectionRect();

        m_x->changeValue( rect.x() );
        m_y->changeValue( rect.y() );
        m_width->changeValue( rect.width() );
        m_height->changeValue( rect.height() );

        m_shearX->setValue(0.0);
        m_shearY->setValue(0.0);
        m_rotate->setValue(0.0);
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

void VTransformDocker::translate()
{
    QPointF newPos( m_x->value(), m_y->value() );

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if( ! selection->count() )
        return;

    QRectF rect = selectionRect();

    if( rect.x() != newPos.x() || rect.y() != newPos.y() )
    {
        QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
        QPointF moveBy = newPos - rect.topLeft();
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

void VTransformDocker::scale()
{
    QSizeF newSize( m_width->value(), m_height->value() );

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    QRectF rect = selectionRect();

    if( rect.width() != newSize.width() || rect.height() != newSize.height() )
    {
        QMatrix resizeMatrix;
        resizeMatrix.translate( rect.x(), rect.y() );
        resizeMatrix.scale( newSize.width() / rect.width(), newSize.height() / rect.height() );
        resizeMatrix.translate( -rect.x(), -rect.y() );

        QList<KoShape*> selectedShapes = selection->selectedShapes( KoFlake::TopLevelSelection );
        QList<QSizeF> oldSizes, newSizes;
        QList<QMatrix> transformations;

        foreach( KoShape* shape, selectedShapes )
        {
            QSizeF oldSize = shape->size();
            QMatrix shapeMatrix = shape->transformationMatrix(0);

            // calculate the matrix we would apply to the local shape matrix
            // that tells us the effective scale values we have to use for the resizing
            QMatrix localMatrix = shapeMatrix * resizeMatrix * shapeMatrix.inverted();
            // save the effective scale values
            double scaleX = localMatrix.m11();
            double scaleY = localMatrix.m22();

            // calculate the scale matrix which is equivalent to our resizing above
            QMatrix scaleMatrix = (QMatrix().scale( scaleX, scaleY ));
            scaleMatrix =  shapeMatrix.inverted() * scaleMatrix * shapeMatrix;

            // calculate the new size of the shape, using the effective scale values
            oldSizes << oldSize;
            newSizes << QSizeF( scaleX * oldSize.width(), scaleY * oldSize.height() );
            // save the rest of the transformation without the resizing part
            transformations << scaleMatrix.inverted() * resizeMatrix;
        }
        QUndoCommand * cmd = new QUndoCommand(i18n("Resize"));
        new KoShapeSizeCommand( selectedShapes, oldSizes, newSizes, cmd );
        new KoShapeTransformCommand( selectedShapes, transformations, cmd );
        canvasController->canvas()->addCommand( cmd );
    }
}

void VTransformDocker::setUnit( KoUnit unit )
{
    enableSignals( false );

    m_x->setUnit( unit );
    m_y->setUnit( unit );
    m_width->setUnit( unit );
    m_height->setUnit( unit );

    enableSignals( true );
}

void VTransformDocker::shear()
{
    double shearX = m_shearX->value();
    double shearY = m_shearY->value();

    if( shearX != 0.0 || shearY != 0.0 )
    {
        QRectF rect = selectionRect();
        QPointF center = rect.center();

        // shear around selection center
        QMatrix shearMatrix;
        shearMatrix.translate( center.x(), center.y() );
        shearMatrix.shear( shearX / rect.height(), shearY / rect.width() );
        shearMatrix.translate( -center.x(), -center.y() );

        m_shearX->setValue( 0.0 );
        m_shearY->setValue( 0.0 );

        KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
        KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
        QUndoCommand * cmd = new KoShapeTransformCommand( selection->selectedShapes(), shearMatrix, 0 );
        cmd->setText( i18n("Shear") );
        canvasController->canvas()->addCommand( cmd );
    }
}

void VTransformDocker::rotate()
{
    double angle = m_rotate->value();

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    QList<KoShape*> selectedShapes = selection->selectedShapes();

    QPointF center = selection->boundingRect().center();

    // rotate around selection center
    QMatrix matrix;
    matrix.translate( center.x(), center.y() );
    matrix.rotate( angle );
    matrix.translate( -center.x(), -center.y() );

    m_rotate->setValue( 0.0 );

    QUndoCommand * cmd = new KoShapeTransformCommand( selectedShapes, matrix, 0 );
    cmd->setText( i18n("Rotate") ); 
    canvasController->canvas()->addCommand( cmd );
}

#include "vtransformdocker.moc"

