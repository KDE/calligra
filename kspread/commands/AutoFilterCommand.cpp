/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "AutoFilterCommand.h"

#include <klocale.h>

#include "CellStorage.h"
#include "Damages.h"
#include "Map.h"
#include "Sheet.h"

#include "database/Database.h"
#include "database/DatabaseManager.h"

using namespace KSpread;

AutoFilterCommand::AutoFilterCommand()
        : AbstractRegionCommand()
{
    setText(i18n("Auto-Filter"));
}

AutoFilterCommand::~AutoFilterCommand()
{
}

void AutoFilterCommand::redo()
{
    Database database(m_sheet->map()->databaseManager()->createUniqueName());
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
