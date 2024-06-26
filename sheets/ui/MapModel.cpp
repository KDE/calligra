/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "MapModel.h"

#include "engine/SheetsDebug.h"

#include "core/Map.h"
#include "core/ModelSupport.h"
#include "core/Sheet.h"
#include "core/SheetModel.h"

#include "commands/SheetCommands.h"

#include <KoIcon.h>

using namespace Calligra::Sheets;

class MapModel::Private
{
public:
    Map *map;

    Sheet *getSheet(int idx) const;

public:
    bool isSheetIndex(const QModelIndex &index, const MapModel *mapModel) const;
};

Sheet *MapModel::Private::getSheet(int idx) const
{
    SheetBase *sb = map->sheet(idx);
    if (!sb)
        return nullptr;
    return dynamic_cast<Sheet *>(sb);
}

bool MapModel::Private::isSheetIndex(const QModelIndex &index, const MapModel *mapModel) const
{
    if (!index.parent().isValid()) {
        return false;
    }
    // If it is a cell, the parent's (the sheet's) model has to be this model.
    if (index.parent().model() != mapModel || index.parent().internalPointer() != map) {
        return false;
    }
    // If it is a cell, the parent (the sheet) has no parent.
    if (index.parent().parent().isValid()) {
        return false;
    }
    // Do not exceed the sheet list.
    if (index.parent().row() >= map->count()) {
        return false;
    }

    Sheet *sheet = getSheet(index.parent().row());
    if (!sheet)
        return false;
    // The index' (the cell's) model has to match the sheet model.
    if (index.model() != sheet->model()) {
        return false;
    }
    return true;
}

MapModel::MapModel(Map *map)
    : QAbstractListModel(map)
    , d(new Private)
{
    d->map = map;
    connect(d->map, &MapBase::sheetAdded, this, &MapModel::addSheet);
    connect(d->map, &MapBase::sheetRemoved, this, &MapModel::removeSheet);
}

MapModel::~MapModel()
{
    delete d;
}

QVariant MapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    // Propagation to sheet model
    if (d->isSheetIndex(index, this)) {
        Sheet *sheet = d->getSheet(index.parent().row());
        if (!sheet)
            return QVariant();
        return sheet->model()->data(index, role);
    }
    if (index.row() >= d->map->count()) {
        return QVariant();
    }
    Sheet *sheet = d->getSheet(index.row());
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QVariant(sheet->sheetName());
    case Qt::DecorationRole:
        return QVariant(koIcon("x-office-spreadsheet"));
    case VisibilityRole:
        return QVariant(!sheet->isHidden());
    case ProtectionRole:
        return QVariant(sheet->isProtected());
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags MapModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    // Propagation to sheet model
    if (d->isSheetIndex(index, this)) {
        Sheet *sheet = d->getSheet(index.parent().row());
        return sheet->model()->flags(index);
    }
    if (index.row() >= d->map->count()) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (!d->map->isProtected()) {
        flags |= Qt::ItemIsSelectable;
        Sheet *sheet = d->getSheet(index.row());
        if (!sheet->isProtected()) {
            flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}

QVariant MapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    if (section == 0 && role == Qt::DisplayRole) {
        return QVariant(i18n("Sheet name"));
    }
    return QVariant();
}

QModelIndex MapModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex index;
    if (parent.isValid()) {
        // If it is a cell, the parent's (the sheet's) model has to be this model.
        if (parent.model() != this || parent.internalPointer() != d->map) {
            return QModelIndex();
        }
        // If it is a cell, the parent (the sheet) has no parent.
        if (parent.parent().isValid()) {
            return QModelIndex();
        }
        // Do not exceed the sheet list.
        if (parent.row() >= d->map->count()) {
            return QModelIndex();
        }
        Sheet *sheet = d->getSheet(index.parent().row());
        index = sheet->model()->index(row, column, parent);
    } else {
        index = createIndex(row, column, d->map);
    }
    return index;
}

int MapModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->map->count();
}

bool MapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Propagation to sheet model
    if (d->isSheetIndex(index, this)) {
        Sheet *sheet = d->getSheet(index.parent().row());
        if (!sheet)
            return false;
        return sheet->model()->setData(index, value, role);
    }

    if (index.isValid() && index.row() < d->map->count()) {
        Sheet *sheet = d->getSheet(index.row());
        switch (role) {
        case Qt::EditRole: {
            const QString name(value.toString());
            if (!name.isEmpty()) {
                KUndo2Command *const command = new RenameSheetCommand(sheet, name);
                Q_EMIT addCommandRequested(command);
                Q_EMIT dataChanged(index, index);
                return true;
            }
            break;
        }
        case VisibilityRole:
            setHidden(sheet, value.toBool());
            break;
        case ProtectionRole:
            break;
        default:
            break;
        }
    }
    return false;
}

bool MapModel::setHidden(Sheet *sheet, bool hidden)
{
    KUndo2Command *command;
    if (hidden && !sheet->isHidden()) {
        command = new HideSheetCommand(sheet);
    } else if (!hidden && sheet->isHidden()) {
        command = new ShowSheetCommand(sheet);
    } else {
        return false; // nothing to do
    }
    Q_EMIT addCommandRequested(command);
    return true;
}

Map *MapModel::map() const
{
    return d->map;
}

void MapModel::addSheet(SheetBase *sheet)
{
    debugSheets << "Added sheet:" << sheet->sheetName();
    Q_EMIT layoutChanged();
}

void MapModel::removeSheet(SheetBase *sheet)
{
    debugSheets << "Removed sheet:" << sheet->sheetName();
    Q_EMIT layoutChanged();
}
