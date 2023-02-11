/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SpellCheck.h"
#include "Actions.h"

#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/Region.h"
#include "engine/ValueStorage.h"
#include "core/Sheet.h"

#include "ui/CellToolBase.h"
#include "ui/commands/DataManipulators.h"

#include <KoCanvasBase.h>

#include <kmessagebox.h>
#include <kstandardaction.h>
#include <sonnet/dialog.h>
#include <sonnet/speller.h>

#include <KLocalizedString>


using namespace Calligra::Sheets;


class SpellChecker::Private
{
public:
    KoCanvasBase* canvasBase;
    Region region;
    CellBase currentCell;
    SheetBase *currentSheet, *firstSheet;
    Sonnet::Speller speller;
    Sonnet::Dialog* dialog;
    KUndo2Command* command;
};

SpellChecker::SpellChecker(QObject *parent)
    : BackgroundChecker(parent)
{
    d = new Private;
    d->dialog = nullptr;
    connect(this, &Sonnet::BackgroundChecker::done, this, &SpellChecker::finishCommand);
}

SpellChecker::~SpellChecker()
{
    delete d->dialog;
    delete d;
}


void SpellChecker::check(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    d->currentSheet = d->firstSheet = sheet;
    d->region = *selection;
    d->canvasBase = selection->canvas();
    d->currentCell = CellBase();
    setSpeller(d->speller);
    if (!d->dialog) {
        d->dialog = new Sonnet::Dialog(this, canvasWidget);
        connect(d->dialog, &Sonnet::Dialog::replace,
                this, [this] (const QString &oldText, int start, const QString &newText) {
                    replace(start, oldText, newText);
                });
    }

    stop();
    start();
}

bool SpellChecker::nextSheet() {
    if (d->currentSheet->map()->count() == 1)
        // Nothing more to do, if there's only one sheet.
        return false;

    // Ask whether we should continue on the next sheet.
    const QString question = i18n("Do you want to check the spelling in the next sheet?");
    if (KMessageBox::questionYesNo(d->canvasBase->canvasWidget(), question) != KMessageBox::Yes)
        return false;

    const MapBase *map = d->currentSheet->map();
    SheetBase *nextSheet = map->nextSheet(d->currentSheet);
    if (nextSheet == d->currentSheet) // We have reached the end, roll to the beginning
        nextSheet = map->sheet(0);
    if (nextSheet == d->firstSheet)   // Reached the original sheet - we're done
        return false;

    d->currentCell = CellBase();
    return true;
}


void SpellChecker::advanceToNextCell()
{
    const ValueStorage *values = d->currentSheet->valueStorage();
    int maxRow = values->rows();

    if (d->region.isSingular()) {  // traversing everything
        int checkFromRow = 0;
        if (d->currentCell.isNull()) {
            checkFromRow = 1;
        } else {
            // existing row
            int row = d->currentCell.row();
            int col = d->currentCell.column();
            int next = 0;
            values->nextInRow(col, row, &next);
            if (next) {
                d->currentCell = CellBase(d->currentSheet, next, row);
                return;
            }
            // Nothing more in this row
            checkFromRow = row + 1;
        }

        // Now we need to find the first valid cell starting from the given row.
        int col = 0;
        for (int row = checkFromRow; row <= maxRow; ++row) {
            values->firstInRow(row, &col);
            if (!col) continue;   // empty row
            d->currentCell = CellBase(d->currentSheet, col, row);
            return;
        }
        d->currentCell = CellBase();
        return;  // No more data
    }

    // traversing the selection only
    QRect sel = d->region.firstRange();
    if (sel.bottom() < maxRow) maxRow = sel.bottom();

    int checkFromRow = 0;
    if (d->currentCell.isNull()) {
        checkFromRow = sel.top();
    } else {
        // existing row
        int row = d->currentCell.row();
        int col = d->currentCell.column();
        int next = 0;
        values->nextInRow(col, row, &next);
        if (next && next <= sel.right()) {
            d->currentCell = CellBase(d->currentSheet, next, row);
            return;
        }
        // Nothing more in this row
        checkFromRow = row + 1;
    }

    // Now we need to find the first valid cell starting from the given row.
    int col = 0;
    for (int row = checkFromRow; row <= maxRow; ++row) {
        if (sel.left() <= 1)
            values->firstInRow(row, &col);
        else
            values->nextInRow(row, sel.left() - 1, &col);
        if (!col) continue;   // empty row

        d->currentCell = CellBase(d->currentSheet, col, row);
        return;
    }
    d->currentCell = CellBase();
}

QString SpellChecker::fetchMoreText()
{
    QString text;
    do {
        advanceToNextCell();

        if (d->currentCell.isNull()) {
            // No more cells. Try the next sheet if we can.
            if (d->region.isSingular() && nextSheet()) continue;

            break;   // We cannot advance, therefore we are done.
        }

        Value value = d->currentCell.value();
        if (!value.isString()) continue;
        QString val = value.asString();
        if (!val.isEmpty()) {
            text = val;
            break;
        }
    } while (!d->currentCell.isNull());

    return text;
}


// TODO - do we want to use the command like this?
void SpellChecker::finishedCurrentFeed()
{
    if (d->dialog->originalBuffer() == d->dialog->buffer()) return;

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

void SpellChecker::finishCommand()
{
    if (d->command) {
        d->canvasBase->addCommand(d->command);
    }
    // TODO Stefan: Save the ignored words in document.
}



SpellCheck::SpellCheck(Actions *actions)
    : CellAction(actions, "tools_spelling", QString(), QIcon(), i18n("Check the spelling"))
{
    checker = new SpellChecker(this);
}

SpellCheck::~SpellCheck()
{
    delete checker;
}

QAction *SpellCheck::createAction() {

    QAction *action = KStandardAction::spelling(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &SpellCheck::triggered);
    action->setToolTip(m_tooltip);
    return action;
}

void SpellCheck::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    checker->check(selection, sheet, canvasWidget);
}




