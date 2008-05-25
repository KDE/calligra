/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonLayerDocker.h"
#include "KarbonLayerModel.h"
#include "KarbonLayerSortingModel.h"

#include <KarbonDocument.h>
#include <KarbonPart.h>
#include <KarbonLayerReorderCommand.h>

#include <KoDocumentSectionView.h>
#include <KoShapeManager.h>
#include <KoShapeBorderModel.h>
#include <KoShapeContainer.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShapeControllerBase.h>
#include <KoSelection.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeReorderCommand.h>
#include <KoShapeLayer.h>
#include <KoShapeGroup.h>

#include <klocale.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <kparts/part.h>

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>

enum ButtonIds
{
    Button_New,
    Button_Raise,
    Button_Lower,
    Button_Delete
};

KarbonLayerDockerFactory::KarbonLayerDockerFactory()
{
}

QString KarbonLayerDockerFactory::id() const
{
    return QString("Layer view");
}

QDockWidget* KarbonLayerDockerFactory::createDockWidget()
{
    KarbonLayerDocker* widget = new KarbonLayerDocker();
    widget->setObjectName(id());

    return widget;
}

KarbonLayerDocker::KarbonLayerDocker()
    : m_part( 0 ), m_model( 0 )
{
    setWindowTitle( i18n( "Layer view" ) );

    QWidget *mainWidget = new QWidget( this );
    QGridLayout* layout = new QGridLayout( mainWidget );
    layout->addWidget( m_layerView = new KoDocumentSectionView( mainWidget ), 0, 0, 1, 4 );

    QButtonGroup *buttonGroup = new QButtonGroup( mainWidget );
    buttonGroup->setExclusive( false );

    QPushButton *button = new QPushButton( mainWidget );
    button->setIcon( SmallIcon( "list-add" ) );
    button->setToolTip( i18n("Add a new layer") );
    buttonGroup->addButton( button, Button_New );
    layout->addWidget( button, 1, 0 );

    button = new QPushButton( mainWidget );
    button->setIcon( SmallIcon( "list-remove" ) );
    button->setToolTip( i18n("Delete selected objects") );
    buttonGroup->addButton( button, Button_Delete );
    layout->addWidget( button, 1, 1 );

    button = new QPushButton( mainWidget );
    button->setIcon( SmallIcon( "go-up" ) );
    button->setToolTip( i18n("Raise selected objects") );
    buttonGroup->addButton( button, Button_Raise );
    layout->addWidget( button, 1, 2 );

    button = new QPushButton( mainWidget );
    button->setIcon( SmallIcon( "go-down" ) );
    button->setToolTip( i18n("Lower selected objects") );
    buttonGroup->addButton( button, Button_Lower );
    layout->addWidget( button, 1, 3 );

    layout->setSpacing( 0 );
    layout->setMargin( 3 );

    setWidget( mainWidget );

    connect( buttonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );

    m_model = new KarbonLayerModel( this );
    m_model->setDocument( m_part ? &m_part->document() : 0 );
    m_sortModel = new KarbonLayerSortingModel(this);
    m_sortModel->setSourceModel( m_model );

    m_layerView->setItemsExpandable( true );
    m_layerView->setModel( m_sortModel );
    m_layerView->setDisplayMode( KoDocumentSectionView::MinimalMode );
    m_layerView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    m_layerView->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_layerView->setDragDropMode( QAbstractItemView::InternalMove );
    m_layerView->setSortingEnabled(true);

    connect( m_layerView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));
}

KarbonLayerDocker::~KarbonLayerDocker()
{
}

void KarbonLayerDocker::updateView()
{
    m_model->update();
}

void KarbonLayerDocker::setPart( KParts::Part * part )
{
    m_part = dynamic_cast<KarbonPart*>( part );
    if( ! m_part )
        m_model->setDocument( 0 );
    else
        m_model->setDocument( &m_part->document() );
}

void KarbonLayerDocker::slotButtonClicked( int buttonId )
{
    switch( buttonId )
    {
        case Button_New:
            addLayer();
            break;
        case Button_Raise:
            raiseItem();
            break;
        case Button_Lower:
            lowerItem();
            break;
        case Button_Delete:
            deleteItem();
            break;
    }
}

void KarbonLayerDocker::itemClicked( const QModelIndex &index )
{
    KoShape *shape = shapeFromIndex( index );
    if( ! shape )
        return;
    if( dynamic_cast<KoShapeLayer*>( shape ) )
        return;

    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();

    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    foreach( KoShape* shape, selection->selectedShapes() )
        shape->update();

    selection->deselectAll();

    foreach( KoShape* shape, selectedShapes )
    {
        if( shape )
        {
            selection->select( shape );
            shape->update();
        }
    }
}

void KarbonLayerDocker::addLayer()
{
    bool ok = true;
    QString name = KInputDialog::getText( i18n( "New Layer" ), i18n( "Enter the name of the new layer:" ),
                                          i18n( "New layer" ), &ok, this );
    if( ok )
    {
        KoShapeLayer* layer = new KoShapeLayer();
        layer->setName( name );
        KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
        QUndoCommand *cmd = new KoShapeCreateCommand( m_part, layer, 0 );
        cmd->setText( i18n( "Create Layer") );
        canvasController->canvas()->addCommand( cmd );
        m_model->update();
    }
}

void KarbonLayerDocker::deleteItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    QUndoCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        if( m_part->document().layers().count() > selectedLayers.count() )
        {
            QList<KoShape*> deleteShapes;
            foreach( KoShapeLayer* layer, selectedLayers )
            {
                deleteShapes += layer->iterator();
                deleteShapes.append( layer );
            }
            cmd = new KoShapeDeleteCommand( m_part, deleteShapes );
            cmd->setText( i18n( "Delete Layer" ) );
        }
        else
        {
            KMessageBox::error( 0L, i18n( "Could not delete all layers. At least one layer is required."), i18n( "Error deleting layers") );
        }
    }
    else if( selectedShapes.count() )
    {
        cmd = new KoShapeDeleteCommand( m_part, selectedShapes );
    }

    if( cmd )
    {
        KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
        canvasController->canvas()->addCommand( cmd );
        m_model->update();
    }
}

void KarbonLayerDocker::raiseItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes, true );

    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();

    QUndoCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        // check if all layers could be raised
        foreach( KoShapeLayer* layer, selectedLayers )
            if( ! m_part->document().canRaiseLayer( layer ) )
                return;

        cmd = new KarbonLayerReorderCommand( &m_part->document(), selectedLayers, KarbonLayerReorderCommand::RaiseLayer );
    }
    else if( selectedShapes.count() )
    {
        cmd = KoShapeReorderCommand::createCommand( selectedShapes, canvas->shapeManager(), KoShapeReorderCommand::RaiseShape );
    }

    if( cmd )
    {
        canvas->addCommand( cmd );
        m_model->update();
    }
}

void KarbonLayerDocker::lowerItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes, true );

    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();

    QUndoCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        // check if all layers could be raised
        foreach( KoShapeLayer* layer, selectedLayers )
            if( ! m_part->document().canLowerLayer( layer ) )
                return;

        cmd = new KarbonLayerReorderCommand( &m_part->document(), selectedLayers, KarbonLayerReorderCommand::LowerLayer );
    }
    else if( selectedShapes.count() )
    {
        cmd = KoShapeReorderCommand::createCommand( selectedShapes, canvas->shapeManager(), KoShapeReorderCommand::LowerShape );
    }

    if( cmd )
    {
        canvas->addCommand( cmd );
        m_model->update();
    }
}

void KarbonLayerDocker::extractSelectedLayersAndShapes( 
        QList<KoShapeLayer*> &layers, QList<KoShape*> &shapes, bool addChilds )
{
    layers.clear();
    shapes.clear();

    QModelIndexList selectedItems = m_layerView->selectionModel()->selectedIndexes();
    if( selectedItems.count() == 0 )
        return;

    // separate selected layers and selected shapes
    foreach( QModelIndex index, selectedItems )
    {
        KoShape *shape = shapeFromIndex( index );
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>( shape );
        if( layer )
            layers.append( layer );
        else if( ! selectedItems.contains( index.parent() ) )
        {
            shapes.append( shape );
            KoShapeGroup * group = dynamic_cast<KoShapeGroup*>( shape );
            if( group && addChilds )
                addChildsRecursive( group, shapes );
        }
    }
}

void KarbonLayerDocker::addChildsRecursive( KoShapeGroup * parent, QList<KoShape*> &shapes )
{
    foreach( KoShape * child, parent->iterator() )
    {
        if( ! shapes.contains( child ) )
            shapes.append( child );
        KoShapeGroup * group = dynamic_cast<KoShapeGroup*>( child );
        if( group )
            addChildsRecursive( group, shapes );
    }
}

KoShape * KarbonLayerDocker::shapeFromIndex( const QModelIndex &index )
{
    Q_ASSERT(index.internalPointer());

    QModelIndex sourceIndex = index;
    if( index.model() != m_model )
        sourceIndex = m_sortModel->mapToSource( index );

    if( ! sourceIndex.isValid() )
        return 0;

    return static_cast<KoShape*>( sourceIndex.internalPointer() );
}

#include "KarbonLayerDocker.moc"

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
