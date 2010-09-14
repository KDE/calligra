/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "SpellCheckCommand.h"

#include "Cell.h"
#include "CellStorage.h"
#include "Map.h"
#include "Sheet.h"
#include "Region.h"
#include "ValueStorage.h"

#include "commands/DataManipulators.h"

#include <KoCanvasBase.h>

#include <KMessageBox>
#include <sonnet/dialog.h>
#include <sonnet/speller.h>

using namespace KSpread;

class SpellCheckCommand::Private
{
public:
    KoCanvasBase* canvasBase;
    int index;
    Region region;
    Cell currentCell;
    Sheet* currentSheet;
    ValueStorage storage;
    Sonnet::Speller speller;
    Sonnet::Dialog* dialog;
    QUndoCommand* command;
};

SpellCheckCommand::SpellCheckCommand(const Region &region, KoCanvasBase* canvasBase)
        : BackgroundChecker(canvasBase->canvasWidget())
        , d(new Private)
{
    d->canvasBase = canvasBase;
    d->index = 0;
    d->region = region;
    d->currentSheet = region.firstSheet();
    if (region.isSingular()) {
        // take the whole sheet
        d->storage = *d->currentSheet->valueStorage();
    } else {
        // only take the selection
        d->storage = d->currentSheet->valueStorage()->subStorage(region);
    }
    setSpeller(d->speller);
    d->dialog = new Sonnet::Dialog(this, canvasBase->canvasWidget());
    d->command = 0;

    connect(this, SIGNAL(done()),
            this, SLOT(finishCommand()));
    connect(d->dialog, SIGNAL(replace(const QString&, int, const QString&)),
            this, SLOT(replace(const QString&, int, const QString&)));
}

SpellCheckCommand::~SpellCheckCommand()
{
    delete d->dialog;
    delete d;
}

QString SpellCheckCommand::fetchMoreText()
{
    QString text;
    // Take the next string value.
    while (d->index < d->storage.count() && text.isEmpty()) {
        const Value value = d->storage.data(d->index);
        if (value.isString()) {
            text = value.asString();
            d->currentCell = Cell(d->currentSheet, d->storage.col(d->index), d->storage.row(d->index));
        }
        d->index++;
    }
    if (text.isEmpty() && d->region.isSingular()) {
        if (d->currentSheet->map()->count() == 1) {
            // Nothing more to do, if there's only one sheet.
            return QString();
        }
        // Ask whether we should continue on the next sheet.
        const QString question = i18n("Do you want to check the spelling in the next sheet?");
        if (KMessageBox::questionYesNo(d->canvasBase->canvasWidget(), question) == KMessageBox::Yes) {
            const Map* map = d->currentSheet->map();
            if (d->currentSheet == map->sheet(map->count() - 1)) {
                // Switch from the last to the first sheet.
                d->currentSheet = map->sheet(0);
            } else {
                // Switch to the next sheet.
                d->currentSheet = map->nextSheet(d->currentSheet);
            }
            if (d->currentSheet == d->region.firstSheet()) {
                // Stop, if reached the starting sheet.
                return QString();
            }
            // Set the storage and reset its index.
            d->index = 0;
            d->storage = *d->currentSheet->valueStorage();
        }
    }
    return text;
}

void SpellCheckCommand::finishedCurrentFeed()
{
    if (d->dialog->originalBuffer() == d->dialog->buffer()) {
        return;
    }
    // TODO Stefan: QUndoCommand-based undo recording for CellStorage.
    if (!d->command) {
        d->command = new QUndoCommand(i18n("Correct Misspelled Words"));
    }
    DataManipulator* command = new DataManipulator(d->command);
    command->setSheet(d->currentSheet);
    command->setValue(Value(d->dialog->buffer()));
    command->setParsing(false);
    command->add(QPoint(d->currentCell.column(), d->currentCell.row()));
    command->setRegisterUndo(false);
}

void SpellCheckCommand::finishCommand()
{
    if (d->command) {
        d->canvasBase->addCommand(d->command);
    }
    deleteLater();
    // TODO Stefan: Save the ignored words in document.
}
