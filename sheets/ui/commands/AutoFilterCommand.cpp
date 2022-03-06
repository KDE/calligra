/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoFilterCommand.h"

#include <KLocalizedString>

#include "CellStorage.h"
#include "Damages.h"
#include "Map.h"
#include "Sheet.h"

#include "database/Database.h"

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
    Database database();
    database.setDisplayFilterButtons(true);
    database.setRange(*this);
    m_sheet->cellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void AutoFilterCommand::undo()
{
    m_sheet->cellStorage()->setDatabase(*this, Database());
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}
