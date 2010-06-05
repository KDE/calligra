/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "MapViewModel.h"

#include "Map.h"
#include "ModelSupport.h"
#include "Sheet.h"

#include "commands/SheetCommands.h"

#include "interfaces/ReadWriteTableModel.h"

#include <KoCanvasBase.h>
#include <KoShapeManager.h>

#include <KIcon>

#define KSPREAD_SHEET_IN_MAP_MODEL_EXTENSION

using namespace KSpread;

class MapViewModel::Private
{
public:
    Sheet* activeSheet;
    KoCanvasBase *canvas;
};


MapViewModel::MapViewModel(Map *map, KoCanvasBase *canvas)
    : MapModel(map)
    , d(new Private)
{
    d->activeSheet = 0;
    d->canvas = canvas;
}

MapViewModel::~MapViewModel()
{
    delete d;
}

QVariant MapViewModel::data(const QModelIndex &index, int role) const
{
    // We handle only this role; the remaining ones go to the MapModel.
    if (role != ActivityRole && role != Qt::CheckStateRole) {
        return MapModel::data(index, role);
    }
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.parent().isValid()) {
        return MapModel::data(index, role);
    }
    if (index.row() >= map()->count()) {
        return QVariant();
    }
    const Sheet* const sheet = map()->sheet(index.row());
    return QVariant(sheet == d->activeSheet);
}

Qt::ItemFlags MapViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    // Propagation to sheet model
    if (index.parent().isValid()) {
        return MapModel::flags(index); // The MapModel takes care of cell indices.
    }
    if (index.row() >= map()->count()) {
        return Qt::NoItemFlags;
    }
    return MapModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool MapViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // We handle only this role; the remaining ones go to the MapModel.
    if (role != ActivityRole && role != Qt::CheckStateRole) {
        return MapModel::setData(index, value, role);
    }
    if (!index.isValid()) {
        return false;
    }
    if (index.parent().isValid()) {
        return MapModel::setData(index, value, role);
    }
    if (index.row() >= map()->count()) {
        return false;
    }
    Sheet* const sheet(map()->sheet(index.row()));
    setActiveSheet(sheet);
    return true;
}

Sheet* MapViewModel::activeSheet() const
{
    return d->activeSheet;
}

void MapViewModel::setActiveSheet(Sheet* sheet)
{
    if (d->activeSheet == sheet) {
        return;
    }
    const QList<Sheet*> list = map()->sheetList();
    const int oldRow = list.indexOf(d->activeSheet);
    const int newRow = list.indexOf(sheet);

    // The sheet may be set to 0 for one exceptional case.
    d->activeSheet = sheet;

    // Unhide, if necessary.
    if (sheet && sheet->isHidden()) {
        QUndoCommand* command = new ShowSheetCommand(sheet);
        d->canvas->addCommand(command);
    }

    // Both sheets have to be in the list. If not, there won't be any signals.
    if (oldRow == -1 || newRow == -1) {
        return;
    }
    const QModelIndex oldIndex(index(oldRow, 0));
    const QModelIndex newIndex(index(newRow, 0));
    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(newIndex, newIndex);
    emit activeSheetChanged(sheet);
}

void MapViewModel::addShape(Sheet *sheet, KoShape *shape)
{
    if (sheet == d->activeSheet) {
        d->canvas->shapeManager()->addShape(shape);
    }
}

void MapViewModel::removeShape(Sheet *sheet, KoShape *shape)
{
    if (sheet == d->activeSheet) {
        d->canvas->shapeManager()->remove(shape);
    }
}

#include "MapViewModel.moc"
