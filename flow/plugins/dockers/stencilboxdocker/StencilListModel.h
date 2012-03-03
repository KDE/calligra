/*
 * Copyright 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef STENCILLISTMODEL_H
#define STENCILLISTMODEL_H

#include <QAbstractListModel>

class KoProperties;
class QList;
class QString;
class QIcon;

/**
 * Struct containing the information of a shape item in a stencil
 */
struct ShapeItem
{
    QString id; // id will be used for shape file path
    QString name;
    QIcon icon;
    KoProperties* properties = 0;
};

class StencilListModel : public QAbstractListModel
{
public:
    StencilListModel(QObject* parent = 0);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
    virtual QStringList mimeTypes() const;

    void appendRow(ShapeItem* item);
    void appendRows(const QList<ShapeItem*> &items);
    void insertRow(int row, ShapeItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    ShapeItem* takeRow(int row);
    ShapeItem* find(const QString &id);
    QModelIndex indexFromItem( const ShapeItem* item) const;

private slots:
    void handleItemChange();

private:
    QList<ShapeItem*> m_list;
    QString m_stencilName;
};

#endif // STENCILLISTMODEL_H
