/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "CQSpreadsheetListModel.h"

#include "CQSpreadsheetCanvas.h"
#include <Map.h>
#include <Sheet.h>

class CQSpreadsheetListModel::Private
{
public:
    Private() : canvas(0), map(0) { }

    CQSpreadsheetCanvas* canvas;
    Calligra::Sheets::Map* map;
};

CQSpreadsheetListModel::CQSpreadsheetListModel(QObject* parent)
    : QAbstractListModel(parent), d(new Private())
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(SheetNameRole, "sheetName");
    setRoleNames(roleNames);
}

CQSpreadsheetListModel::~CQSpreadsheetListModel()
{
    delete d;
}

QVariant CQSpreadsheetListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || !d->map)
        return QVariant();

    switch(role) {
        case SheetNameRole:
            return d->map->sheet(index.row())->sheetName();
        default:
            break;
    }

    return QVariant();
}

int CQSpreadsheetListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if(d->map) {
        return d->map->count();
    }

    return 0;
}

QObject* CQSpreadsheetListModel::canvas() const
{
    return d->canvas;
}
void CQSpreadsheetListModel::setCanvas(QObject* canvas)
{
    if (d->canvas != canvas) {
        d->canvas = qobject_cast<CQSpreadsheetCanvas*>(canvas);
        Q_ASSERT(d->canvas);
        if(d->map) {
            beginRemoveRows(QModelIndex(), 0, d->map->count());
            endRemoveRows();
        }
        d->map = d->canvas->documentMap();
        beginInsertRows(QModelIndex(), 0, d->map->count());
        endInsertRows();
        emit canvasChanged();
    }
}
