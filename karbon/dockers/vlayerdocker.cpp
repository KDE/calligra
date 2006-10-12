/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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

#include "vlayerdocker.h"

#include <QGridLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QAbstractItemModel>

#include <klocale.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kmessagebox.h>

#include <KoDocumentSectionView.h>
#include <KoShapeManager.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vdocument.h"
#include "vcanvas.h"
#include "vlayer.h"
#include "vlayercmd.h"

enum ButtonIds
{
    Button_New,
    Button_Raise,
    Button_Lower,
    Button_Delete
};

VLayerDocker::VLayerDocker( KarbonView *view, VDocument *doc )
: QWidget( view )
, m_view( view )
, m_document( doc )
, m_model( 0 )
{
    setObjectName("Layers");

    QGridLayout* layout = new QGridLayout;
    layout->addWidget( m_layerView = new KoDocumentSectionView( this ), 0, 0, 1, 5 );

    QButtonGroup *buttonGroup = new QButtonGroup( this );
    buttonGroup->setExclusive( false );

    QToolButton *button = new QToolButton( this );
    button->setIcon( SmallIcon( "14_layer_newlayer" ) );
    button->setText( i18n( "New" ) );
    button->setToolTip( i18n("Add a new layer") );
    buttonGroup->addButton( button, Button_New );
    layout->addWidget( button, 1, 0 );

    button = new QToolButton( this );
    button->setIcon( SmallIcon( "14_layer_raiselayer" ) );
    button->setText( i18n( "Raise" ) );
    button->setToolTip( i18n("Raise selected objects") );
    buttonGroup->addButton( button, Button_Raise );
    layout->addWidget( button, 1, 1 );

    button = new QToolButton( this );
    button->setIcon( SmallIcon( "14_layer_lowerlayer" ) );
    button->setText( i18n( "Lower" ) );
    button->setToolTip( i18n("Lower selected objects") );
    buttonGroup->addButton( button, Button_Lower );
    layout->addWidget( button, 1, 2 );

    button = new QToolButton( this );
    button->setIcon( SmallIcon( "14_layer_deletelayer" ) );
    button->setText( i18n( "Delete" ) );
    button->setToolTip( i18n("Delete selected objects") );
    buttonGroup->addButton( button, Button_Delete );
    layout->addWidget( button, 1, 3 );

    layout->setSpacing( 0 );
    layout->setMargin( 3 );

    connect( buttonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );

    layout->activate();
    setLayout(layout);

    m_canvas = qobject_cast<KarbonCanvas*>( m_view->canvas() );
    m_model = new VDocumentModel( m_document, m_canvas->shapeManager() );
    m_layerView->setItemsExpandable( true );
    m_layerView->setModel( m_model );
    m_layerView->setDisplayMode( KoDocumentSectionView::MinimalMode );
    m_layerView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    m_layerView->setSelectionBehavior( QAbstractItemView::SelectRows );

    connect( m_layerView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));
}

VLayerDocker::~VLayerDocker()
{
}

void VLayerDocker::updateView()
{
    m_model->update();
}

void VLayerDocker::slotButtonClicked( int buttonId )
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

void VLayerDocker::itemClicked( const QModelIndex &index )
{
    kDebug() << "entering VLayerDocker::itemClicked" << endl;
    Q_ASSERT(index.internalPointer());

    if( ! index.isValid() )
        return;

    KoShape *shape = static_cast<KoShape*>( index.internalPointer() );
    if( ! shape )
        return;
    if( dynamic_cast<KoLayerShape*>( shape ) )
        return;

    QList<KoLayerShape*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    KoSelection *selection = m_canvas->shapeManager()->selection();

    QRectF oldSelectionRect = selection->boundingRect();

    selection->deselectAll();

    foreach( KoShape* shape, selectedShapes )
    {
        if( shape )
            selection->select( shape );
    }

    m_canvas->updateCanvas( oldSelectionRect.unite( selection->boundingRect() ) );
    kDebug() << "leaving VLayerDocker::itemClicked" << endl;
}

void VLayerDocker::addLayer()
{
    bool ok = true;
    QString name = KInputDialog::getText( i18n( "New Layer" ), i18n( "Enter the name of the new layer:" ),
                                          i18n( "New layer" ), &ok, this );
    if( ok )
    {
        KoLayerShape* layer = new KoLayerShape();
        m_view->part()->commandHistory()->addCommand( new VLayerCreateCmd( m_document, layer ), true );
        m_document->setObjectName( layer, name );
        m_model->update();
    }
}

void VLayerDocker::deleteItem()
{
    QList<KoLayerShape*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    KCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        if( m_document->layers().count() > selectedLayers.count() )
            cmd = new VLayerDeleteCmd( m_document, m_view->part(), selectedLayers );
        else
        {
            KMessageBox::error( 0L, i18n( "Could not delete all layers. At least one layer is requiered."), i18n( "Error deleting layers") );
        }
    }
    else if( selectedShapes.count() )
    {
        cmd = new KoShapeDeleteCommand( m_view->part(), selectedShapes.toSet() );
    }

    if( cmd )
    {
        m_view->part()->commandHistory()->addCommand( cmd, true );
        m_model->update();
    }
}

void VLayerDocker::raiseItem()
{
    QList<KoLayerShape*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    KCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        // check if all layers could be raised
        foreach( KoLayerShape* layer, selectedLayers )
            if( ! m_document->canRaiseLayer( layer ) )
                return;

        cmd = new VLayerZOrderCmd( m_document, selectedLayers, VLayerZOrderCmd::raiseLayer );
    }
    else if( selectedShapes.count() )
    {
        // TODO implement shape z-order command
    }

    if( cmd )
    {
        m_view->part()->commandHistory()->addCommand( cmd, true );
        m_model->update();
    }
}

void VLayerDocker::lowerItem()
{
    QList<KoLayerShape*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes( selectedLayers, selectedShapes );

    KCommand *cmd = 0;

    if( selectedLayers.count() )
    {
        // check if all layers could be raised
        foreach( KoLayerShape* layer, selectedLayers )
            if( ! m_document->canLowerLayer( layer ) )
                return;

        cmd = new VLayerZOrderCmd( m_document, selectedLayers, VLayerZOrderCmd::lowerLayer );
    }
    else if( selectedShapes.count() )
    {
        // TODO implement shape z-order command
    }

    if( cmd )
    {
        m_view->part()->commandHistory()->addCommand( cmd, true );
        m_model->update();
    }
}

void VLayerDocker::extractSelectedLayersAndShapes( QList<KoLayerShape*> &layers, QList<KoShape*> &shapes )
{
    layers.clear();
    shapes.clear();

    QModelIndexList selectedItems = m_layerView->selectionModel()->selectedIndexes();
    if( selectedItems.count() == 0 )
        return;

    // separate selected layers and selected shapes
    foreach( QModelIndex index, selectedItems )
    {
        KoShape *shape = static_cast<KoShape*>( index.internalPointer() );
        KoLayerShape *layer = dynamic_cast<KoLayerShape*>( shape );
        if( layer )
            layers.append( layer );
        else if( ! selectedItems.contains( index.parent() ) )
            shapes.append( shape );
    }
}

VDocumentModel::VDocumentModel( VDocument *document, KoShapeManager *shapeManager )
: m_document( document )
, m_shapeManager( shapeManager )
{
}

void VDocumentModel::update()
{
    emit layoutChanged();
}

int VDocumentModel::rowCount( const QModelIndex &parent ) const
{
    // check if parent is root node
    if( ! parent.isValid() )
        return m_document->layers().count();

    Q_ASSERT(parent.model() == this);
    Q_ASSERT(parent.internalPointer());

    KoShapeContainer *parentShape = dynamic_cast<KoShapeContainer*>( (KoShape*)parent.internalPointer() );
    if( ! parentShape )
        return 0;

    return parentShape->childCount();
}

int VDocumentModel::columnCount( const QModelIndex & ) const
{
    return 1;
}

QModelIndex VDocumentModel::index( int row, int column, const QModelIndex &parent ) const
{
    // check if parent is root node
    if( ! parent.isValid() )
    {
        if( row < m_document->layers().count() )
            return createIndex( row, column, m_document->layers().at(m_document->layers().count()-1-row) );
        else
            return QModelIndex();
    }

    Q_ASSERT(parent.model() == this);
    Q_ASSERT(parent.internalPointer());

    KoShapeContainer *parentShape = dynamic_cast<KoShapeContainer*>( (KoShape*)parent.internalPointer() );
    if( ! parentShape )
        return QModelIndex();

    if( row < parentShape->childCount() )
        return createIndex( row, column, parentShape->iterator().at(row) );
    else
        return QModelIndex();
}

QModelIndex VDocumentModel::parent( const QModelIndex &child ) const
{
    // check if child is root node
    if( ! child.isValid() )
        return QModelIndex();

    Q_ASSERT(child.model() == this);
    Q_ASSERT(child.internalPointer());

    KoShape *childShape = static_cast<KoShape*>( child.internalPointer() );
    // check if child shape is a layer, and return invalid model index if it is
    KoLayerShape *childlayer = dynamic_cast<KoLayerShape*>( childShape );
    if( childlayer )
        return QModelIndex();

    // get the childs parent shape
    KoShapeContainer *parentShape = childShape->parent();
    if( ! parentShape )
        return QModelIndex();

    // check if the parent is a layer
    KoLayerShape *parentLayer = dynamic_cast<KoLayerShape*>( parentShape );
    if( parentLayer )
        return createIndex( m_document->layers().count()-1-m_document->layers().indexOf( parentLayer ), 0, parentShape );

    // get the grandparent to determine the row of the parent shape
    KoShapeContainer *grandParentShape = parentShape->parent();
    if( ! grandParentShape )
        return QModelIndex();

    return createIndex( grandParentShape->iterator().indexOf( parentShape ), 0, parentShape );
}

QVariant VDocumentModel::data( const QModelIndex &index, int role ) const
{
    if( ! index.isValid() )
        return QVariant();

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    KoShape *shape = static_cast<KoShape*>( index.internalPointer() );

    switch (role)
    {
        case Qt::DisplayRole:
        {
            QString name = m_document->objectName( shape );
            if( name.isEmpty() )
            {
                if( dynamic_cast<KoLayerShape*>( shape ) )
                    name = i18n("Layer");
                else if( dynamic_cast<KoShapeContainer*>( shape ) )
                    name = i18n("Group");
                else
                    name = i18n("Shape");
            }
            return name;
        }
        case Qt::DecorationRole: return QVariant();//return shape->icon();
        case Qt::EditRole: return m_document->objectName( shape );
        case Qt::SizeHintRole: return shape->size();
        case ActiveRole:
        {
            KoLayerShape *layer = dynamic_cast<KoLayerShape*>( shape );
            if( layer )
                return (layer == m_document->activeLayer() );
            else if( m_shapeManager )
                return m_shapeManager->selection()->isSelected( shape );
            else
                return false;
        }
        case PropertiesRole: return QVariant::fromValue( properties( shape ) );
        case AspectRatioRole:
        {
            QRectF bbox = shape->boundingRect();
            return double(bbox.width()) / bbox.height();
        }
        default:
            // TODO draw layers and shape into an image for the thumbnail and tooltip
/*            if (role >= int(BeginThumbnailRole))
                return QImage( role - int(BeginThumbnailRole), role - int(BeginThumbnailRole), QImage::Format_ARGB32 );
            else*/
                return QVariant();
    }
}

Qt::ItemFlags VDocumentModel::flags(const QModelIndex &index) const
{
    if( ! index.isValid() )
        return Qt::ItemIsEnabled;

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
    if( dynamic_cast<KoShapeContainer*>( (KoShape*)index.internalPointer() ) )
        flags |= Qt::ItemIsDropEnabled;
    return flags;
}

bool VDocumentModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    if( ! index.isValid() )
        return false;

    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalPointer());

    KoShape *shape = static_cast<KoShape*>( index.internalPointer() );
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            m_document->setObjectName( shape, value.toString() );
            break;
        case PropertiesRole:
            setProperties( shape, value.value<PropertyList>());
            break;
        case ActiveRole:
            if (value.toBool())
            {
                KoLayerShape *layer = dynamic_cast<KoLayerShape*>( shape );
                if( layer )
                    m_document->setActiveLayer( layer );
            }
            break;
        default:
            return false;
    }

    emit dataChanged( index, index );
    return true;
}

KoDocumentSectionModel::PropertyList VDocumentModel::properties( KoShape* shape ) const
{
    PropertyList l;
    l << Property(i18n("Visible"), SmallIcon("14_layer_visible"), SmallIcon("14_layer_novisible"), shape->isVisible());
    l << Property(i18n("Locked"), SmallIcon("locked"), SmallIcon("unlocked"), shape->isLocked());
    return l;
}

void VDocumentModel::setProperties( KoShape* shape, const PropertyList &properties )
{
    bool oldVisibleState = shape->isVisible();
    bool oldLockedState = shape->isLocked();

    shape->setVisible( properties.at( 0 ).state.toBool() );
    shape->setLocked( properties.at( 1 ).state.toBool() );

    if( ( oldVisibleState != shape->isVisible() ) || ( oldLockedState != shape->isLocked() ) )
        shape->repaint();
}

#include "vlayerdocker.moc"

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
