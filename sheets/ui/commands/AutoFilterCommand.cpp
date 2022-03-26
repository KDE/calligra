/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoFilterCommand.h"

#include "engine/Damages.h"
#include "engine/MapBase.h"
#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "core/Database.h"

using namespace Calligra::Sheets;

AutoFilterCommand::AutoFilterCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Auto-Filter"));
}

AutoFilterCommand::~AutoFilterCommand()
{
}

void AutoFilterCommand::redo()
{
    Database database;
    database.setDisplayFilterButtons(true);
    database.setRange(*this);
    m_sheet->fullCellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void AutoFilterCommand::undo()
{
    m_sheet->fullCellStorage()->setDatabase(*this, Database());
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}
