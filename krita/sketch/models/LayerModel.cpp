/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "LayerModel.h"
#include "LayerThumbProvider.h"
#include <kis_node_model.h>
#include <kis_view2.h>
#include <kis_canvas2.h>
#include <kis_node_manager.h>
#include <kis_dummies_facade_base.h>
#include <kis_doc2.h>
#include <kis_node.h>
#include <kis_image.h>
#include <kis_layer.h>
#include <kis_group_layer.h>
#include <kis_paint_layer.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoProperties.h>
#include <QDeclarativeEngine>

class LayerModel::Private {
public:
    Private(LayerModel* qq)
        : q(qq)
        , nodeModel(new KisNodeModel(qq))
        , aboutToRemoveRoots(false)
        , canvas(0)
        , nodeManager(0)
        , image(0)
        , activeNode(0)
        , declarativeEngine(0)
        , thumbProvider(0)
    {}

    LayerModel* q;
    QList<KisNodeSP> layers;
    KisNodeModel* nodeModel;
    bool aboutToRemoveRoots;
    KisView2* view;
    KisCanvas2* canvas;
    QPointer<KisNodeManager> nodeManager;
    KisImageWSP image;
    KisNodeSP activeNode;
    QDeclarativeEngine* declarativeEngine;
    LayerThumbProvider* thumbProvider;

    static int counter()
    {
        static int count = 0;
        return count++;
    }
    
    static QStringList layerClassNames()
    {
        QStringList list;
        list << "KisGroupLayer";
        list << "KisLayer";
        return list;
    }

    int deepChildCount(KisNodeSP layer)
    {
        quint32 childCount = layer->childCount();
        QList<KisNodeSP> children = layer->childNodes(layerClassNames(), KoProperties());
        for(quint32 i = 0; i < childCount; ++i)
            childCount += deepChildCount(children.at(i));
        return childCount;
    }

    void rebuildLayerList(KisNodeSP layer = 0)
    {
        if(layer == 0)
        {
            layers.clear();
            layer = image->rootLayer();
        }
        // implementation node: The root node is not a visible node, and so
        // is never added to the list of layers
        QList<KisNodeSP> children = layer->childNodes(layerClassNames(), KoProperties());
        if(layer->childCount() == 0)
            return;
        for(quint32 i = layer->childCount(); i > 0; --i)
        {
            layers << children.at(i-1);
            rebuildLayerList(children.at(i-1));
        }
    }
};

LayerModel::LayerModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    QHash<int, QByteArray> roles;
    roles[IconRole] = "icon";
    roles[NameRole] = "name";
    roles[ActiveLayerRole] = "activeLayer";
    roles[OpacityRole] = "opacity";
    roles[PercentOpacityRole] = "percentOpacity";
    roles[VisibleRole] = "visible";
    roles[LockedRole] = "locked";
    roles[CompositeDetailsRole] = "compositeDetails";
    roles[FilterRole] = "filter";
    roles[ChildCountRole] = "childCount";
    roles[DeepChildCountRole] = "deepChildCount";
    roles[DepthRole] = "depth";
    roles[PreviousItemDepthRole] = "previousItemDepth";
    roles[NextItemDepthRole] = "nextItemDepth";
    setRoleNames(roles);

    connect(d->nodeModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), 
            this, SLOT(source_rowsAboutToBeInserted(QModelIndex, int, int)));
    connect(d->nodeModel, SIGNAL(rowsInserted(QModelIndex, int, int)), 
            this, SLOT(source_rowsInserted(QModelIndex, int, int)));

    connect(d->nodeModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), 
            this, SLOT(source_rowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(d->nodeModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), 
            this, SLOT(source_rowsRemoved(QModelIndex, int, int)));

    connect(d->nodeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), 
            this, SLOT(source_dataChanged(QModelIndex,QModelIndex)));
    connect(d->nodeModel, SIGNAL(modelReset()), 
            this, SLOT(source_modelReset()));

    connect(d->nodeModel, SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    connect(d->nodeModel, SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
}

LayerModel::~LayerModel()
{
    delete d;
}

QObject* LayerModel::view() const
{
    return d->view;
}

void LayerModel::setView(QObject *newView)
{
    KisView2* view = qobject_cast<KisView2*>(newView);
    if (d->canvas) {
        d->nodeModel->setDummiesFacade(0, 0);

        disconnect(d->image, 0, this, 0);
        disconnect(d->nodeManager, 0, this, 0);
        disconnect(d->nodeModel, 0, d->nodeManager, 0);
        disconnect(d->nodeModel, SIGNAL(nodeActivated(KisNodeSP)), this, SLOT(currentNodeChanged(KisNodeSP)));
    }

    d->view = view;
    if (!d->view)
    {
        qDebug() << "The view is not a view, on attempting to set the view on the layer model!";
        d->canvas = 0;
        d->image = 0;
        d->nodeManager = 0;
        return;
    }

    d->canvas = view->canvasBase();
    d->thumbProvider = new LayerThumbProvider();
    d->thumbProvider->setLayerModel(this);
    d->thumbProvider->setLayerID(Private::counter());
    d->declarativeEngine->addImageProvider(QString("layerthumb%1").arg(d->thumbProvider->layerID()), d->thumbProvider);

    if (d->canvas) {
        d->image = d->canvas->view()->image();
        d->nodeManager = d->canvas->view()->nodeManager();

        KisDummiesFacadeBase *kritaDummiesFacade = dynamic_cast<KisDummiesFacadeBase*>(d->canvas->view()->document()->shapeController());
        d->nodeModel->setDummiesFacade(kritaDummiesFacade, d->image);

        connect(d->image, SIGNAL(sigAboutToBeDeleted()), SLOT(notifyImageDeleted()));

        // cold start
        currentNodeChanged(d->nodeManager->activeNode());

        // Connection KisNodeManager -> KisLayerBox
        connect(d->nodeManager, SIGNAL(sigUiNeedChangeActiveNode(KisNodeSP)), this, SLOT(currentNodeChanged(KisNodeSP)));

        // Connection KisLayerBox -> KisNodeManager
        // The order of these connections is important! See comment in the ctor
        connect(d->nodeModel, SIGNAL(nodeActivated(KisNodeSP)), d->nodeManager, SLOT(slotUiActivatedNode(KisNodeSP)));
        connect(d->nodeModel, SIGNAL(nodeActivated(KisNodeSP)), SLOT(currentNodeChanged(KisNodeSP)));

        // Node manipulation methods are forwarded to the node manager
        connect(d->nodeModel, SIGNAL(requestAddNode(KisNodeSP, KisNodeSP, KisNodeSP)),
                d->nodeManager, SLOT(addNodeDirect(KisNodeSP, KisNodeSP, KisNodeSP)));
        connect(d->nodeModel, SIGNAL(requestMoveNode(KisNodeSP, KisNodeSP, KisNodeSP)),
                d->nodeManager, SLOT(moveNodeDirect(KisNodeSP, KisNodeSP, KisNodeSP)));
        d->rebuildLayerList();
        reset();
    }
}

QObject* LayerModel::engine() const
{
    return d->declarativeEngine;
}

void LayerModel::setEngine(QObject* newEngine)
{
    d->declarativeEngine = qobject_cast<QDeclarativeEngine*>(newEngine);
    emit engineChanged();
}

void LayerModel::currentNodeChanged(KisNodeSP newActiveNode)
{
    if (d->activeNode)
    {
        QModelIndex oldIndex = d->nodeModel->indexFromNode(d->activeNode);
        source_dataChanged(oldIndex, oldIndex);
    }
    d->activeNode = newActiveNode;
    if (d->activeNode)
    {
        QModelIndex oldIndex = d->nodeModel->indexFromNode(d->activeNode);
        source_dataChanged(oldIndex, oldIndex);
    }
}

QVariant LayerModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if (index.isValid())
    {
        index.internalPointer();
        KisNodeSP node = d->layers.at(index.row());
        KisNodeSP parent;
        int depth = -1;
        switch(role)
        {
        case IconRole:
            if(dynamic_cast<const KisGroupLayer*>(node.constData()))
                data = QLatin1String("../images/svg/icon-layer_group-red.svg");
            else if(!dynamic_cast<const KisPaintLayer*>(node.constData()))
                data = QLatin1String("../images/svg/icon-layer_filter-red.svg");
            else
                data = QString("image://layerthumb%1/%2").arg(d->thumbProvider->layerID()).arg(index.row());
            //data = "image://color/0.9,0.9,0.9,1.0";
            break;
        case NameRole:
            data = node->name();
            break;
        case ActiveLayerRole:
            data = (node == d->activeNode);
            break;
        case OpacityRole:
            data = node->opacity();
            break;
        case PercentOpacityRole:
            data = node->percentOpacity();
            break;
        case VisibleRole:
            data = node->visible();
            break;
        case LockedRole:
            data = node->userLocked();
            break;
        case CompositeDetailsRole:
            // composite op goes here...
            data = node->compositeOp()->description();
            break;
        case FilterRole:
            break;
        case ChildCountRole:
            data = node->childCount();
            break;
        case DeepChildCountRole:
            data = d->deepChildCount(d->layers.at(index.row()));
            break;
        case DepthRole:
            parent = node;
            while(parent)
            {
                ++depth;
                parent = parent->parent();
            }
            data = depth;
            break;
        case PreviousItemDepthRole:
            if(index.row() == 0)
                data = -1;
            else
            {
                parent = d->layers.at(index.row() - 1);
                while(parent)
                {
                    ++depth;
                    parent = parent->parent();
                }
                data = depth;
            }
            break;
        case NextItemDepthRole:
            if(index.row() == d->layers.count() - 1)
                data = -1;
            else
            {
                parent = d->layers.at(index.row() + 1);
                while(parent)
                {
                    ++depth;
                    parent = parent->parent();
                }
                data = depth;
            }
            break;
        default:
            break;
        }
    }
    return data;
}

int LayerModel::rowCount(const QModelIndex& parent) const
{
    if ( parent.isValid() )
        return 0;

    return d->layers.count();
}

QVariant LayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

void LayerModel::setActive(int index)
{
    if(index > -1 && index < d->layers.count())
    {
        KisNodeSP newNode = d->layers.at(index);
        d->nodeManager->slotUiActivatedNode(newNode);
        currentNodeChanged(newNode);
    }
}

void LayerModel::setLocked(int index, bool newLocked)
{
    if(index > -1 && index < d->layers.count())
    {
        d->layers[index]->setUserLocked(newLocked);
        QModelIndex idx = createIndex(index, 0);
        dataChanged(idx, idx);
    }
}

void LayerModel::setOpacity(int index, float newOpacity)
{
    if(index > -1 && index < d->layers.count())
    {
        d->layers[index]->setOpacity(newOpacity);
        QModelIndex idx = createIndex(index, 0);
        dataChanged(idx, idx);
    }
}

void LayerModel::setVisible(int index, bool newVisible)
{
    if(index > -1 && index < d->layers.count())
    {
        d->layers[index]->setVisible(newVisible);
        QModelIndex idx = createIndex(index, 0);
        dataChanged(idx, idx);
    }
}

QImage LayerModel::layerThumbnail(QString layerID) const
{
    int index = layerID.toInt();
    QImage thumb;
    if(index > -1 && index < d->layers.count())
    {
        if(d->thumbProvider)
            thumb = d->layers[index]->createThumbnail(120, 120);
    }
    return thumb;
}

void LayerModel::source_rowsAboutToBeInserted(QModelIndex /*p*/, int /*from*/, int /*to*/)
{
//     if ( !p.isValid() )
//         return;
// 
//     int f = d->proxyRowFromSourceIndex(p);
//     int t = f + (from-to);
//     beginInsertRows(QModelIndex(), f, t);
    beginResetModel();
}

void LayerModel::source_rowsInserted(QModelIndex /*p*/, int, int)
{
//     if ( !p.isValid() )
//         return;
// 
//     endInsertRows();
    d->rebuildLayerList();
    endResetModel();
}

void LayerModel::source_rowsAboutToBeRemoved(QModelIndex /*p*/, int /*from*/, int /*to*/)
{
//     int f = d->proxyRowFromSourceIndex(p);
//     int t = f + (from-to);
//     beginRemoveRows(QModelIndex(), f, t);
    beginResetModel();
}

void LayerModel::source_rowsRemoved(QModelIndex, int, int)
{
//     endRemoveRows();
    d->rebuildLayerList();
    endResetModel();
}

void LayerModel::source_dataChanged(QModelIndex /*tl*/, QModelIndex /*br*/)
{
    QModelIndex top = createIndex(0, 0);
    QModelIndex bottom = createIndex(d->layers.count() - 1, 0);
    dataChanged(top, bottom);
}

void LayerModel::source_modelReset()
{
    beginResetModel();
    d->rebuildLayerList();
    endResetModel();
}

void LayerModel::notifyImageDeleted()
{
    setView(d->view);
}

#include "LayerModel.moc"
