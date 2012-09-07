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

#include <QAbstractListModel>
#include <QImage>
#include <kis_types.h>

class LayerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(QObject* engine READ engine WRITE setEngine NOTIFY engineChanged)
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
        FilterRole,
        ChildCountRole,
        DeepChildCountRole,
        DepthRole,
        PreviousItemDepthRole,
        NextItemDepthRole
    };
    explicit LayerModel(QObject* parent = 0);
    virtual ~LayerModel();

    QObject* view() const;
    void setView(QObject* newView);
    QObject* engine() const;
    void setEngine(QObject* newEngine);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Q_INVOKABLE void setActive(int index);
    Q_INVOKABLE void setOpacity(int index, float newOpacity);
    Q_INVOKABLE void setVisible(int index, bool newVisible);
    Q_INVOKABLE void setLocked(int index, bool newLocked);
    QImage layerThumbnail(QString layerID) const;

Q_SIGNALS:
    void viewChanged();
    void engineChanged();

private slots:
    void source_rowsAboutToBeInserted(QModelIndex, int, int);
    void source_rowsAboutToBeRemoved(QModelIndex, int, int);
    void source_rowsInserted(QModelIndex, int, int);
    void source_rowsRemoved(QModelIndex, int, int);
    void source_dataChanged(QModelIndex, QModelIndex);
    void source_modelReset();
    void currentNodeChanged(KisNodeSP newActiveNode);
    void notifyImageDeleted();

private:
    class Private;
    Private* d;
};

#endif // LAYERMODEL_H
