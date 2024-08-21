/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "MapViewModel.h"

#include "core/Map.h"
#include "core/ModelSupport.h"
#include "core/Sheet.h"

#include "commands/SheetCommands.h"

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoShape.h>
#include <KoShapeManager.h>

#include <KXMLGUIClient>

#include <QActionGroup>

using namespace Calligra::Sheets;

class MapViewModel::Private
{
public:
    Sheet *activeSheet;
    KoCanvasBase *canvas;
    KXMLGUIClient *xmlGuiClient;
    QActionGroup *gotoSheetActionGroup;
};

MapViewModel::MapViewModel(Map *map, KoCanvasBase *canvas, KXMLGUIClient *xmlGuiClient)
    : MapModel(map)
    , d(new Private)
{
    d->activeSheet = nullptr;
    d->canvas = canvas;
    d->xmlGuiClient = xmlGuiClient;
    d->gotoSheetActionGroup = new QActionGroup(this);

    connect(d->gotoSheetActionGroup, &QActionGroup::triggered, this, &MapViewModel::gotoSheetActionTriggered);

    // Add the initial controlled sheets.
    const QList<SheetBase *> sheets = map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        Sheet *sheet = dynamic_cast<Sheet *>(sheets[i]);
        if (sheet)
            addSheet(sheet);
    }
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
    SheetBase *basesheet = map()->sheet(index.row());
    Sheet *sheet = dynamic_cast<Sheet *>(basesheet);
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
    SheetBase *basesheet(map()->sheet(index.row()));
    Sheet *sheet = dynamic_cast<Sheet *>(basesheet);
    setActiveSheet(sheet);
    return true;
}

Sheet *MapViewModel::activeSheet() const
{
    return d->activeSheet;
}

void MapViewModel::setActiveSheet(Sheet *sheet)
{
    if (d->activeSheet == sheet) {
        return;
    }
    const QList<SheetBase *> list = map()->sheetList();
    const int oldRow = list.indexOf(d->activeSheet);
    const int newRow = list.indexOf(sheet);

    // The sheet may be set to 0 for one exceptional case.
    d->activeSheet = sheet;

    if (!sheet) {
        return;
    }

    // Unhide, if necessary.
    if (sheet->isHidden()) {
        KUndo2Command *command = new ShowSheetCommand(sheet);
        d->canvas->addCommand(command);
    }

    // Check the appropriate action of the goto sheet action group.
    const QList<QAction *> actions = d->gotoSheetActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i) {
        if (actions[i]->iconText() == sheet->sheetName()) {
            actions[i]->setChecked(true);
            break;
        }
    }

    // Both sheets have to be in the list. If not, there won't be any signals.
    if (oldRow == -1 || newRow == -1) {
        return;
    }
    const QModelIndex oldIndex(index(oldRow, 0));
    const QModelIndex newIndex(index(newRow, 0));
    Q_EMIT dataChanged(oldIndex, oldIndex);
    Q_EMIT dataChanged(newIndex, newIndex);
    Q_EMIT activeSheetChanged(sheet);
}

void MapViewModel::addSheet(SheetBase *sheet)
{
    MapModel::addSheet(sheet);
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);

    connect(fullSheet, &Sheet::shapeAdded, this, &MapViewModel::addShape);
    connect(fullSheet, &Sheet::shapeRemoved, this, &MapViewModel::removeShape);

    if (!d->xmlGuiClient) {
        return;
    }

    // Update the goto sheet action group
    const QString name = sheet->sheetName();
    QAction *action = new QAction(koIcon("x-office-spreadsheet"), name, this);
    action->setCheckable(true);
    action->setToolTip(i18nc("Activate sheet named foo", "Activate %1", name));

    d->gotoSheetActionGroup->addAction(action);

    const QList<QAction *> actions = d->gotoSheetActionGroup->actions();
    d->xmlGuiClient->unplugActionList("go_goto_sheet_actionlist");
    d->xmlGuiClient->plugActionList("go_goto_sheet_actionlist", actions);
}

void MapViewModel::removeSheet(SheetBase *sheet)
{
    MapModel::removeSheet(sheet);
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);

    disconnect(fullSheet, &Sheet::shapeAdded, this, &MapViewModel::addShape);
    disconnect(fullSheet, &Sheet::shapeRemoved, this, &MapViewModel::removeShape);

    if (!d->xmlGuiClient) {
        return;
    }

    // Update the goto sheet action group
    QAction *action = nullptr;
    const QList<QAction *> actions = d->gotoSheetActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i) {
        if (actions[i]->text() == sheet->sheetName()) {
            action = actions[i];
            break;
        }
    }
    if (action) {
        d->gotoSheetActionGroup->removeAction(action);
        const QList<QAction *> actions = d->gotoSheetActionGroup->actions();
        d->xmlGuiClient->unplugActionList("go_goto_sheet_actionlist");
        d->xmlGuiClient->plugActionList("go_goto_sheet_actionlist", actions);
    }
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

void MapViewModel::gotoSheetActionTriggered(QAction *action)
{
    const QList<QAction *> actions = d->gotoSheetActionGroup->actions();
    for (int i = 0; i < actions.count(); ++i) {
        if (actions[i]->text() == action->text()) {
            SheetBase *basesheet = map()->findSheet(action->iconText());
            Sheet *sheet = basesheet ? dynamic_cast<Sheet *>(basesheet) : nullptr;
            if (sheet) {
                setActiveSheet(sheet);
            } else { // should not happen
                d->gotoSheetActionGroup->removeAction(action);
            }
            break;
        }
    }
}
