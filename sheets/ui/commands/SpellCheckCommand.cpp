/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SpellCheckCommand.h"

#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/Region.h"
#include "engine/ValueStorage.h"
#include "core/Sheet.h"

#include "DataManipulators.h"

#include <KoCanvasBase.h>

#include <kmessagebox.h>
#include <sonnet/dialog.h>
#include <sonnet/speller.h>

using namespace Calligra::Sheets;

class SpellCheckCommand::Private
{
public:
    KoCanvasBase* canvasBase;
    int index;
    Region region;
    CellBase currentCell;
    SheetBase *currentSheet;
    ValueStorage storage;
    Sonnet::Speller speller;
    Sonnet::Dialog* dialog;
    KUndo2Command* command;
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

    connect(this, &Sonnet::BackgroundChecker::done,
            this, &SpellCheckCommand::finishCommand);
    connect(d->dialog, &Sonnet::Dialog::replace,
            this, [this] (const QString &oldText, int start, const QString &newText) {
                replace(start, oldText, newText);
            });
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
            d->currentCell = CellBase(d->currentSheet, d->storage.col(d->index), d->storage.row(d->index));
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
            const MapBase *map = d->currentSheet->map();
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
    // TODO Stefan: KUndo2Command-based undo recording for CellStorage.
    if (!d->command) {
        d->command = new KUndo2Command(kundo2_i18n("Correct Misspelled Words"));
    }
    DataManipulator* command = new DataManipulator(d->command);
    Sheet *sheet = dynamic_cast<Sheet *>(d->currentSheet);
    command->setSheet(sheet);
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
