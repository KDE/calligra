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
#include "core/Sheet.h"
#include "core/Database.h"



using namespace Calligra::Sheets;


AutoFilter::AutoFilter(Actions *actions)
    : CellAction(actions, "autoFilter", i18n("&Auto-Filter"), koIcon("view-filter"), i18n("Add an automatic filter to a cell range"))
{
}

AutoFilter::~AutoFilter()
{
}

void AutoFilter::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect range = selection->lastRange();
    if ((range.width() == 1) && (range.height() == 1)) {
        CellBase cell(sheet, range.left(), range.top());
        range = extendSelectionToRange(cell, false);
        selection->initialize(range, sheet);
        selection->emitModified();
    }

    AutoFilterCommand* command = new AutoFilterCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->execute(selection->canvas());
}


AutoFilterCommand::AutoFilterCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Auto-Filter"));
}

AutoFilterCommand::~AutoFilterCommand()
{
}

bool AutoFilterCommand::performCommands()
{
    Database database;
    database.setDisplayFilterButtons(true);
    database.setRange(*this);
    m_sheet->fullCellStorage()->setDatabase(*this, database);
    return true;
}

