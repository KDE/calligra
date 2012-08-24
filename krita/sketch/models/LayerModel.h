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

#ifndef LAYERMODEL_H
#define LAYERMODEL_H

#include <QAbstractProxyModel>
#include <kis_types.h>

class LayerModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged)
public:
    enum LayerRoles {
        IconRole = Qt::UserRole + 1,
        NameRole,
        ActiveLayerRole,
        OpacityRole,
        PercentOpacityRole,
        VisibleRole,
        LockedRole,
        CompositeDetailsRole,
        FilterRole
    };
    explicit LayerModel(QObject* parent = 0);
    virtual ~LayerModel();

    QObject* view() const;
    void setView(QObject* newView);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & p = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual QModelIndex mapFromSource(const QModelIndex &) const;
    virtual QModelIndex mapToSource(const QModelIndex &) const;

    virtual QModelIndex parent(const QModelIndex &) const;
    virtual QModelIndex index(int, int, const QModelIndex & p = QModelIndex()) const;

Q_SIGNALS:
    void viewChanged();

private slots:
    void source_rowsAboutToBeInserted(QModelIndex, int, int);
    void source_rowsAboutToBeRemoved(QModelIndex, int, int);
    void source_rowsInserted(QModelIndex, int, int);
    void source_rowsRemoved(QModelIndex, int, int);
    void source_dataChanged(QModelIndex, QModelIndex);
    void source_modelReset();
    void currentNodeChanged(KisNodeSP newActiveNode);

private:
    class Private;
    Private* d;
};

#endif // LAYERMODEL_H
