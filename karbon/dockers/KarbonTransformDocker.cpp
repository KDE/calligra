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

#include "KarbonTransformDocker.h"

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
#include <kdebug.h>
#include <QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QMatrix>
#include <QtGui/QToolTip>
#include <QtGui/QGridLayout>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

KarbonTransformDockerFactory::KarbonTransformDockerFactory()
{
}

QString KarbonTransformDockerFactory::id() const
{
    return QString("Transform");
}

QDockWidget* KarbonTransformDockerFactory::createDockWidget()
{
    KarbonTransformDocker* widget = new KarbonTransformDocker();
    widget->setObjectName(id());

    return widget;
}

KoDockFactory::DockPosition KarbonTransformDockerFactory::defaultDockPosition() const
{
    return DockMinimized;
}

class KarbonTransformDocker::Private
{
public:
    KoUnitDoubleSpinBox * x;
    KoUnitDoubleSpinBox * y;
    KoUnitDoubleSpinBox * width;
    KoUnitDoubleSpinBox * height;
    QDoubleSpinBox * rotate;
    QDoubleSpinBox * shearX;
    QDoubleSpinBox * shearY;

    QLabel * xLabel;
    QLabel * yLabel;
    QLabel * wLabel;
    QLabel * hLabel;
    QLabel * rLabel;
    QLabel * sxLabel;
    QLabel * syLabel;

    QGridLayout * mainLayout;
};

KarbonTransformDocker::KarbonTransformDocker()
    : d( new Private() )
{
    setWindowTitle( i18n( "Transform" ) );

    QWidget * mainWidget = new QWidget( this );

    //TODO: Set 5000 limit to real Karbon14 limit
    const double limit = 5000.0;

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoUnit documentUnit = canvasController->canvas()->unit();

    //X:
    d->xLabel = new QLabel( i18n ( "X:" ), mainWidget );
    d->x = new KoUnitDoubleSpinBox( mainWidget );
    d->x->setMinMaxStep( -limit, limit, 1.0 );
    d->x->setUnit( documentUnit );
    d->x->setDecimals( 1 );
    d->x->setToolTip( i18n("Set x-position of actual selection") );
    connect( d->x, SIGNAL( editingFinished() ), this, SLOT( translate() ) );

    //Y:
    d->yLabel = new QLabel( i18n ( "Y:" ), mainWidget );
    d->y = new KoUnitDoubleSpinBox( mainWidget );
    d->y->setMinMaxStep( -limit, limit, 1.0 );
    d->y->setUnit( documentUnit );
    d->y->setDecimals( 1 );
    d->y->setToolTip( i18n("Set y-position of actual selection") );
    connect( d->y, SIGNAL( editingFinished() ), this, SLOT( translate() ) );

    //Width:
    d->wLabel = new QLabel( i18n ( "W:" ), mainWidget );
    d->width = new KoUnitDoubleSpinBox( mainWidget );
    d->width->setMinMaxStep( 0.0, limit, 1.0 );
    d->width->setUnit( documentUnit );
    d->width->setDecimals( 1 );
    d->width->setToolTip( i18n("Set width of actual selection") );
    connect( d->width, SIGNAL( editingFinished() ), this, SLOT( scale() ) );

    //Height:
    d->hLabel = new QLabel( i18n ( "H:" ), mainWidget );
    d->height = new KoUnitDoubleSpinBox( mainWidget );
    d->height->setMinMaxStep( 0.0, limit, 1.0 );
    d->height->setUnit( documentUnit );
    d->height->setDecimals( 1 );
    d->height->setToolTip( i18n("Set height of actual selection") );
    connect( d->height, SIGNAL( editingFinished() ), this, SLOT( scale() ) );

    //ROTATE:
    d->rLabel = new QLabel( i18n ( "R:" ), mainWidget );
    d->rotate = new QDoubleSpinBox( mainWidget );
    d->rotate->setMinimum (-360.0);
    d->rotate->setMaximum (360.0);
    d->rotate->setSingleStep(1.0);
    d->rotate->setValue(10.0);
    d->rotate->setDecimals(1);
    d->rotate->setToolTip( i18n("Rotate actual selection") );
    connect( d->rotate, SIGNAL( editingFinished() ), this, SLOT( rotate() ) );

    //X-Shear:
    d->sxLabel = new QLabel( i18n ( "SX:" ), mainWidget );
    d->shearX = new QDoubleSpinBox( mainWidget );
    d->shearX->setMinimum (-360.0);
    d->shearX->setMaximum (360.0);
    d->shearX->setSingleStep(1.0);
    d->shearX->setValue(10.0);
    d->shearX->setDecimals(1);

    d->shearX->setToolTip( i18n("Shear actual selection in x-direction") );
    connect( d->shearX, SIGNAL( editingFinished() ), this, SLOT( shear() ) );

    //Y-Shear:
    d->syLabel = new QLabel( i18n ( "SY:" ), mainWidget );
    d->shearY = new QDoubleSpinBox( mainWidget );
    d->shearY->setMinimum (-360.0);
    d->shearY->setMaximum (360.0);
    d->shearY->setSingleStep(1.0);
    d->shearY->setValue(10.0);
    d->shearY->setDecimals(1);
    d->shearY->setToolTip( i18n("Shear actual selection in y-direction") );
    connect( d->shearY, SIGNAL( editingFinished() ), this, SLOT( shear() ) );

    const int minimumWidth = 60;
    d->x->setMinimumWidth( minimumWidth );
    d->y->setMinimumWidth( minimumWidth );
    d->width->setMinimumWidth( minimumWidth );
    d->height->setMinimumWidth( minimumWidth );
    d->rotate->setMinimumWidth( minimumWidth );
    d->shearX->setMinimumWidth( minimumWidth );
    d->shearY->setMinimumWidth( minimumWidth );

    setWidget( mainWidget );

    d->mainLayout = new QGridLayout( mainWidget );

    layoutVertical();

    update();

    connect( this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(dockLocationChanged(Qt::DockWidgetArea)) );
}

KarbonTransformDocker::~KarbonTransformDocker()
{
    delete d;
}

void KarbonTransformDocker::layoutVertical()
{
    clearLayout();

    d->mainLayout->addWidget( d->xLabel, 0, 0 );
    d->mainLayout->addWidget( d->x, 0, 1 );
    d->mainLayout->addWidget( d->yLabel, 0, 2 );
    d->mainLayout->addWidget( d->y, 0, 3 );
    d->mainLayout->addWidget( d->wLabel, 1, 0 );
    d->mainLayout->addWidget( d->width, 1, 1 );
    d->mainLayout->addWidget( d->hLabel, 1, 2 );
    d->mainLayout->addWidget( d->height, 1, 3 );
    d->mainLayout->addWidget( d->rLabel, 3, 0 );
    d->mainLayout->addWidget( d->rotate, 3, 1 );
    d->mainLayout->addWidget( d->sxLabel, 2, 0 );
    d->mainLayout->addWidget( d->shearX, 2, 1 );
    d->mainLayout->addWidget( d->syLabel, 2, 2 );
    d->mainLayout->addWidget( d->shearY, 2, 3 );
    d->mainLayout->setRowStretch( 4, 1 );
    d->mainLayout->setColumnStretch( 1, 1 );
    d->mainLayout->setColumnStretch( 3, 1 );
}

void KarbonTransformDocker::layoutHorizontal()
{
    clearLayout();

    d->mainLayout->addWidget( d->xLabel, 0, 0 );
    d->mainLayout->addWidget( d->x, 0, 1 );
    d->mainLayout->addWidget( d->yLabel, 0, 2 );
    d->mainLayout->addWidget( d->y, 0, 3 );
    d->mainLayout->addWidget( d->wLabel, 0, 4 );
    d->mainLayout->addWidget( d->width, 0, 5 );
    d->mainLayout->addWidget( d->hLabel, 0, 6 );
    d->mainLayout->addWidget( d->height, 0, 7 );
    d->mainLayout->addWidget( d->rLabel, 0, 8 );
    d->mainLayout->addWidget( d->rotate, 0, 9 );
    d->mainLayout->addWidget( d->sxLabel, 0, 10 );
    d->mainLayout->addWidget( d->shearX, 0, 11 );
    d->mainLayout->addWidget(d->syLabel, 0, 12 );
    d->mainLayout->addWidget( d->shearY, 0, 13 );
    d->mainLayout->setRowStretch( 4, 1 );
    d->mainLayout->setColumnStretch( 1, 0 );
    d->mainLayout->setColumnStretch( 3, 0 );
    d->mainLayout->setColumnStretch( 14, 1 );
}

void KarbonTransformDocker::clearLayout()
{
    d->mainLayout->removeWidget( d->xLabel );
    d->mainLayout->removeWidget( d->x );
    d->mainLayout->removeWidget( d->yLabel );
    d->mainLayout->removeWidget( d->y );
    d->mainLayout->removeWidget( d->wLabel );
    d->mainLayout->removeWidget( d->width );
    d->mainLayout->removeWidget( d->hLabel );
    d->mainLayout->removeWidget( d->height );
    d->mainLayout->removeWidget( d->rLabel );
    d->mainLayout->removeWidget( d->rotate );
    d->mainLayout->removeWidget( d->sxLabel );
    d->mainLayout->removeWidget( d->syLabel );
    d->mainLayout->removeWidget( d->shearY );
}

void KarbonTransformDocker::enableSignals( bool enable )
{
    d->x->blockSignals( ! enable );
    d->y->blockSignals( ! enable );
    d->width->blockSignals( ! enable );
    d->height->blockSignals( ! enable );
    d->shearX->blockSignals( ! enable );
    d->shearY->blockSignals( ! enable );
    d->rotate->blockSignals( ! enable );
}

QRectF KarbonTransformDocker::selectionRect()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    QRectF aaBB;
    QList<KoShape*> selectedShapes = selection->selectedShapes();
    foreach( KoShape * shape, selectedShapes )
    {
        if( aaBB.isEmpty() )
            aaBB = shape->absoluteTransformation(0).map( shape->outline() ).boundingRect();
        else
            aaBB = aaBB.united( shape->absoluteTransformation(0).map( shape->outline() ).boundingRect() );
    }
   return aaBB;
}

void KarbonTransformDocker::update()
{
    enableSignals( false );

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if ( selection->count() > 0 )
    {
        enableWidgets( true );
        QRectF rect = selectionRect();

        d->x->changeValue( rect.x() );
        d->y->changeValue( rect.y() );
        d->width->changeValue( rect.width() );
        d->height->changeValue( rect.height() );

        d->shearX->setValue(0.0);
        d->shearY->setValue(0.0);
        d->rotate->setValue(0.0);
    }
    else
    {
        d->x->changeValue(0.0);
        d->y->changeValue(0.0);
        d->width->changeValue(0.0);
        d->height->changeValue(0.0);
        d->shearX->setValue(0.0);
        d->shearY->setValue(0.0);
        d->rotate->setValue(0.0);
        enableWidgets( false );
    }

    enableSignals( true );
}

void KarbonTransformDocker::translate()
{
    QPointF newPos( d->x->value(), d->y->value() );

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

void KarbonTransformDocker::scale()
{
    QSizeF newSize( d->width->value(), d->height->value() );

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
        QList<QMatrix> oldState;
        QList<QMatrix> newState;

        foreach( KoShape* shape, selectedShapes )
        {
            QSizeF oldSize = shape->size();
            oldState << shape->transformation();
            QMatrix shapeMatrix = shape->absoluteTransformation(0);

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
            // apply the rest of the transformation without the resizing part
            shape->applyAbsoluteTransformation( scaleMatrix.inverted() * resizeMatrix );
            newState << shape->transformation();
        }
        QUndoCommand * cmd = new QUndoCommand(i18n("Resize"));
        new KoShapeSizeCommand( selectedShapes, oldSizes, newSizes, cmd );
        new KoShapeTransformCommand( selectedShapes, oldState, newState, cmd );
        canvasController->canvas()->addCommand( cmd );
    }
}

void KarbonTransformDocker::setUnit( KoUnit unit )
{
    enableSignals( false );

    d->x->setUnit( unit );
    d->y->setUnit( unit );
    d->width->setUnit( unit );
    d->height->setUnit( unit );

    enableSignals( true );
}

void KarbonTransformDocker::shear()
{
    double shearX = d->shearX->value();
    double shearY = d->shearY->value();

    if( shearX != 0.0 || shearY != 0.0 )
    {
        QRectF rect = selectionRect();
        QPointF center = rect.center();

        // shear around selection center
        QMatrix shearMatrix;
        shearMatrix.translate( center.x(), center.y() );
        shearMatrix.shear( shearX / rect.height(), shearY / rect.width() );
        shearMatrix.translate( -center.x(), -center.y() );

        d->shearX->setValue( 0.0 );
        d->shearY->setValue( 0.0 );

        KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
        KoSelection * selection = canvasController->canvas()->shapeManager()->selection();

        QList<QMatrix> oldTransforms, newTransforms;
        foreach( KoShape * shape, selection->selectedShapes() )
        {
            oldTransforms << shape->transformation();
            shape->applyAbsoluteTransformation( shearMatrix );
            newTransforms << shape->transformation();
        }
        QUndoCommand * cmd = new KoShapeTransformCommand( selection->selectedShapes(), oldTransforms, newTransforms );
        cmd->setText( i18n("Shear") );
        canvasController->canvas()->addCommand( cmd );
    }
}

void KarbonTransformDocker::rotate()
{
    double angle = d->rotate->value();

    KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection * selection = canvasController->canvas()->shapeManager()->selection();
    QList<KoShape*> selectedShapes = selection->selectedShapes();

    QPointF center = selection->boundingRect().center();

    // rotate around selection center
    QMatrix matrix;
    matrix.translate( center.x(), center.y() );
    matrix.rotate( angle );
    matrix.translate( -center.x(), -center.y() );

    d->rotate->setValue( 0.0 );

    QList<QMatrix> oldTransforms, newTransforms;
    foreach( KoShape * shape, selectedShapes )
    {
        oldTransforms << shape->transformation();
        shape->applyAbsoluteTransformation( matrix );
        newTransforms << shape->transformation();
    }

    QUndoCommand * cmd = new KoShapeTransformCommand( selectedShapes, oldTransforms, newTransforms );
    cmd->setText( i18n("Rotate") ); 
    canvasController->canvas()->addCommand( cmd );
}

void KarbonTransformDocker::dockLocationChanged( Qt::DockWidgetArea area )
{
    kDebug() << "docking at area" << area;

    if( area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea )
        layoutVertical();
    else
        layoutHorizontal();
}

void KarbonTransformDocker::enableWidgets( bool enable )
{
    d->x->setEnabled( enable );
    d->y->setEnabled( enable );
    d->width->setEnabled( enable );
    d->height->setEnabled( enable );
    d->rotate->setEnabled( enable );
    d->shearX->setEnabled( enable );
    d->shearY->setEnabled( enable );
}

#include "KarbonTransformDocker.moc"

