/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoFilter.h"
#include "Actions.h"

#include <KLocalizedString>

#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/Database.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

AutoFilter::AutoFilter(Actions *actions)
    : CellAction(actions, "autoFilter", i18n("&Auto-Filter"), koIcon("view-filter"), i18n("Add an automatic filter to a cell range"))
{
}

AutoFilter::~AutoFilter() = default;

void AutoFilter::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect range = selection->lastRange();
    if ((range.width() == 1) && (range.height() == 1)) {
        CellStorage *cs = sheet->fullCellStorage();
        QVector<QPair<QRectF, Database>> databases = cs->databases(*selection);
        if (databases.length()) {
            range = databases[0].first.toRect();
        } else {
            CellBase cell(sheet, range.left(), range.top());
            range = extendSelectionToRange(cell, false);
        }
        selection->initialize(range, sheet);
        selection->emitModified();
    }

    AutoFilterCommand *command = new AutoFilterCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->execute(selection->canvas());
}

AutoFilterCommand::AutoFilterCommand()
    : AbstractRegionCommand()
{
    setText(kundo2_i18n("Auto-Filter"));
}

AutoFilterCommand::~AutoFilterCommand() = default;

bool AutoFilterCommand::performCommands()
{
    CellStorage *cs = m_sheet->fullCellStorage();
    QVector<QPair<QRectF, Database>> databases = cs->databases(*this);
    // If there is a database that matches exactly, we use that one
    // The execute() method above has already tried to find one if no range is provided.
    QRect mine = lastRange();
    for (auto e : databases) {
        if (e.first == mine) {
            // We have a database to use.
            bool display = e.second.displayFilterButtons();
            e.second.setDisplayFilterButtons(!display);
            cs->setDatabase(*this, e.second);
            return true;
        }
    }

    // Nothing matched exactly, make a new one.
    Database database;
    database.setDisplayFilterButtons(true);
    database.setRange(*this);
    cs->setDatabase(*this, database);
    return true;
}
