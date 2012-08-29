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
#include <kis_node_model.h>
#include <kis_view2.h>
#include <kis_canvas2.h>
#include <kis_node_manager.h>
#include <kis_dummies_facade_base.h>
#include <kis_doc2.h>
#include <kis_node.h>
#include <kis_image.h>
#include <KoShapeBasedDocumentBase.h>

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
    {}

    LayerModel* q;
    KisNodeModel* nodeModel;
    bool aboutToRemoveRoots;
    KisView2* view;
    KisCanvas2* canvas;
    QPointer<KisNodeManager> nodeManager;
    KisImageWSP image;
    KisNodeSP activeNode;

    int source_deepRowCount(QModelIndex index = QModelIndex(), int rowDepth = 0)
    {
        qDebug() << "counting at depth" << rowDepth;
        if (!q->sourceModel())
            return 0;

        int count = 0;
        if (index.isValid())
            count++;
        for(int i = 0; i < q->sourceModel()->rowCount(index); ++i)
        {
            count += source_deepRowCount(q->sourceModel()->index(i, 0, index), rowDepth + 1);
            qDebug() << "Current count:" << count;
        }
        return count;
    }

    QModelIndex sourceIndexFromProxyRow(int searchRow, int& rowsRemaining, QModelIndex searchParent)
    {
        if (rowsRemaining <= 0)
        {
            qDebug() << "This will not do! Fail :P";
            return QModelIndex();
        }
        qDebug() << "locating index, rows remainins:" << rowsRemaining;
        QModelIndex foundIt;

        for(int i = 0; q->sourceModel()->rowCount(searchParent); ++i)
        {
            --rowsRemaining;
            QModelIndex test = q->sourceModel()->index(i, 0, searchParent);
            if (rowsRemaining == 0)
            {
                qDebug() << "located index!";
                foundIt = test;
                break;
            }
            if (q->sourceModel()->rowCount(test) > 0)
            {
                foundIt = sourceIndexFromProxyRow(searchRow, rowsRemaining, test);
                if (foundIt.isValid())
                    break;
            }
        }

        return foundIt;
    }

    int proxyRowFromSourceIndex(QModelIndex index)
    {
        int currentRow = -1;
        QModelIndex currentSearchParent;
        proxyRowFromSourceIndexActual(index, currentRow, currentSearchParent);
        return currentRow;
    }

    bool proxyRowFromSourceIndexActual(QModelIndex index, int currentRow, QModelIndex currentSearchParent)
    {
        bool found = false;
        for(int i = 0; i < q->sourceModel()->rowCount(currentSearchParent); ++i)
        {
            ++currentRow;
            QModelIndex test = q->sourceModel()->index(i, 0, currentSearchParent);
            if (test == index)
            {
                found = true;
                break;
            }
            if (q->sourceModel()->rowCount(test) > 0)
            {
                if (proxyRowFromSourceIndexActual(index, currentRow, test))
                {
                    found = true;
                    break;
                }
            }
        }
        return found;
    }
};

LayerModel::LayerModel(QObject* parent)
    : QAbstractProxyModel(parent)
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
    setRoleNames(roles);
    
    setSourceModel(d->nodeModel);
    connect(sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), 
            this, SLOT(source_rowsAboutToBeInserted(QModelIndex, int, int)));
    connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex, int, int)), 
            this, SLOT(source_rowsInserted(QModelIndex, int, int)));

    connect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), 
            this, SLOT(source_rowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex, int, int)), 
            this, SLOT(source_rowsRemoved(QModelIndex, int, int)));

    connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), 
            this, SLOT(source_dataChanged(QModelIndex,QModelIndex)));
    connect(sourceModel(), SIGNAL(modelReset()), 
            this, SLOT(source_modelReset()));

    connect(sourceModel(), SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    connect(sourceModel(), SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
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
        qDebug() << "view is fail!";
        d->canvas = 0;
        d->image = 0;
        d->nodeManager = 0;
        return;
    }
    qDebug() << "view exists, continuing";

    d->canvas = view->canvasBase();
    if (d->canvas) {
        qDebug() << "canvas exists";
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
        reset();
    }
    qDebug() << "new view was set successfully and stuff!" << rowCount(QModelIndex());
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
        qDebug() << "index is valid...";
        index.internalPointer();
        KisNodeSP node = d->nodeModel->nodeFromIndex(mapToSource(index));
        
        switch(role)
        {
        case IconRole:
            // node->createThumbnail will be useful here...
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
            break;
        case FilterRole:
            break;
        default:
            break;
        }
    }
    return data;
}

QVariant LayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex LayerModel::mapFromSource(const QModelIndex & source) const
{
    if (!sourceModel())
        return QModelIndex();

    if (!source.parent().isValid())
        return QModelIndex();

    return index(d->proxyRowFromSourceIndex(source), 0, QModelIndex());
}

QModelIndex LayerModel::mapToSource(const QModelIndex & proxy) const
{
    if ( !sourceModel() )
        return QModelIndex();

    int remainingRows = proxy.row();
    return d->sourceIndexFromProxyRow(remainingRows, remainingRows, QModelIndex());
}

QModelIndex LayerModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QModelIndex LayerModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

int LayerModel::rowCount(const QModelIndex& parent) const
{
    if ( parent.isValid() )
        return 0;
    if ( !sourceModel() )
        return 0;

    return d->source_deepRowCount();
}

int LayerModel::columnCount(const QModelIndex & p) const
{
    if ( p.isValid() )
        return 0;
    if ( !sourceModel() )
        return 0;
    return 1;
}

void LayerModel::source_rowsAboutToBeInserted(QModelIndex p, int from, int to)
{
    if ( !p.isValid() )
        return;

    int f = d->proxyRowFromSourceIndex(p);
    int t = f + (from-to);
    beginInsertRows(QModelIndex(), f, t);
}

void LayerModel::source_rowsInserted(QModelIndex p, int, int)
{
    if ( !p.isValid() )
        return;

    endInsertRows();
}

void LayerModel::source_rowsAboutToBeRemoved(QModelIndex p, int from, int to)
{
    int f = d->proxyRowFromSourceIndex(p);
    int t = f + (from-to);
    beginRemoveRows(QModelIndex(), f, t);
}

void LayerModel::source_rowsRemoved(QModelIndex, int, int)
{
    endRemoveRows();
}

void LayerModel::source_dataChanged(QModelIndex tl, QModelIndex br)
{
    QModelIndex p_tl = mapFromSource(tl);
    QModelIndex p_br = mapFromSource(br);
    emit dataChanged(p_tl, p_br);
}

void LayerModel::source_modelReset()
{
    reset();
}

#include "LayerModel.moc"
