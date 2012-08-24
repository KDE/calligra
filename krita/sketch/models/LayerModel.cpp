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
#include <kis_node.h>

class LayerModel::Private {
public:
    Private(LayerModel* qq)
        : q(qq)
        , nodeModel(new KisNodeModel(qq))
        , aboutToRemoveRoots(false)
    {}

    LayerModel* q;
    KisNodeModel* nodeModel;
    QHash<int, int> sourceRootsCounts; 
    bool aboutToRemoveRoots;

    int source_root2count(int row)
    {
        if (!q->sourceModel())
            return 0;

        if (!sourceRootsCounts.contains(row))
        {
            sourceRootsCounts[row] = q->sourceModel()->rowCount(
                q->sourceModel()->index(row,0)
            );
        }

        return sourceRootsCounts[row];
    }

    int row_fromSource(int root, int row)
    {
        for (int r =0; r< root; r++)
           row += source_root2count(r);
        return row;
    }

    QPair<int,int> row_toSource(int row)
    {
        int root = 0;
        for (int r =0; r < q->sourceModel()->rowCount(); r++)
        {
            root = r;
            int rows_in_root = source_root2count(r);
            if (row >= rows_in_root)
                row -= rows_in_root;
            else break;
        }

        return qMakePair(root, row);
    }
};

LayerModel::LayerModel(QObject* parent)
    : QAbstractProxyModel(parent)
    , d(new Private(this))
{
    QHash<int, QByteArray> roles;
    roles[IconRole] = "icon";
    roles[NameRole] = "name";
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

QVariant LayerModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(index.isValid())
    {
        KisNodeSP node = d->nodeModel->nodeFromIndex(mapToSource(index));
        
        switch(role)
        {
        case IconRole:
            // node->createThumbnail will be useful here...
            break;
        case NameRole:
            data = node->name();
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

    return index(d->row_fromSource(source.parent().row(), source.row()), source.column());
}

QModelIndex LayerModel::mapToSource(const QModelIndex & proxy) const
{
    if( !sourceModel() )
        return QModelIndex();

    QPair<int, int> pos = d->row_toSource(proxy.row());
    int root_row = pos.first;
    int row = pos.second;

    QModelIndex p = sourceModel()->index(root_row, proxy.column());
    return sourceModel()->index(row, proxy.column(), p);
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
    if( parent.isValid() )
        return 0;
    if( !sourceModel() )
        return 0;

    int count = 0;
    for (int root_row =0; root_row< sourceModel()->rowCount(); root_row++)
        count += d->source_root2count(root_row);

    return count;
}

int LayerModel::columnCount(const QModelIndex & p) const
{
    if( p.isValid() )
        return 0;
    if( !sourceModel() )
        return 0;
    return 1;
}

void LayerModel::source_rowsAboutToBeInserted(QModelIndex p, int from, int to)
{
    if( !p.isValid() )
        return;

    int f = d->row_fromSource(p.row(), from);
    int t = f + (from-to);
    beginInsertRows(QModelIndex(), f, t);
}

void LayerModel::source_rowsInserted(QModelIndex p, int, int)
{
    d->sourceRootsCounts.clear();
    if( !p.isValid() )
        return;

    endInsertRows();
}

void LayerModel::source_rowsAboutToBeRemoved(QModelIndex p, int from, int to)
{
    if( !p.isValid() )
    {
        //remove root items
        int f = d->row_fromSource(from,0);
        int t = d->row_fromSource(to,0)+ d->source_root2count(to);

        if( f != t )
        {
            beginRemoveRows(QModelIndex(), f, t-1);
            d->aboutToRemoveRoots = true;
        }

        return;
    }

    int f = d->row_fromSource(p.row(), from);
    int t = f + (from-to);
    beginRemoveRows(QModelIndex(), f, t);
}

void LayerModel::source_rowsRemoved(QModelIndex p, int, int)
{
    d->sourceRootsCounts.clear();

    if( !p.isValid() )
    {
        //remove root items
        if (d->aboutToRemoveRoots)
        {
            d->aboutToRemoveRoots = false;
            endRemoveRows();
        }
        return;
    }

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
    d->sourceRootsCounts.clear();
    reset();
}

#include "LayerModel.moc"
