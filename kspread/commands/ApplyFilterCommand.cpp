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

#include "ApplyFilterCommand.h"

#include <klocale.h>

#include "CellStorage.h"
#include "Damages.h"
#include "Doc.h"
#include "Sheet.h"

#include "database/DatabaseRange.h"

using namespace KSpread;

ApplyFilterCommand::ApplyFilterCommand()
    : AbstractRegionCommand()
{
    setText(i18n("Apply Filter"));
}

ApplyFilterCommand::~ApplyFilterCommand()
{
}

void ApplyFilterCommand::redo()
{    m_database.applyFilter();
    m_sheet->doc()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void ApplyFilterCommand::undo()
{
}

void ApplyFilterCommand::setDatabase(const DatabaseRange& database)
{
    m_database = database;
}
