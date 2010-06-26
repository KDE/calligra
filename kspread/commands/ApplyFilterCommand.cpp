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
#include "Map.h"
#include "Sheet.h"
#include "RowColumnFormat.h"

#include "database/Database.h"
#include "database/Filter.h"

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
{
    m_undoData.clear();
    Database database = m_database;

    Sheet* const sheet = database.range().lastSheet();
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database.orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i) {
        const bool isFiltered = !database.filter().evaluate(database, i);
//         kDebug() <<"Filtering column/row" << i <<"?" << isFiltered;
        if (database.orientation() == Qt::Vertical) {
            m_undoData[i] = sheet->rowFormat(i)->isFiltered();
            sheet->nonDefaultRowFormat(i)->setFiltered(isFiltered);
        } else { // database.orientation() == Qt::Horizontal
            m_undoData[i] = sheet->columnFormat(i)->isFiltered();
            sheet->nonDefaultColumnFormat(i)->setFiltered(isFiltered);
        }
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    m_sheet->cellStorage()->setDatabase(*this, Database());
    m_sheet->cellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void ApplyFilterCommand::undo()
{
    Database database = m_database;
    database.setFilter(*m_oldFilter);

    Sheet* const sheet = database.range().lastSheet();
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database.orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i) {
        if (database.orientation() == Qt::Vertical)
            sheet->nonDefaultRowFormat(i)->setFiltered(m_undoData[i]);
        else // database.orientation() == Qt::Horizontal
            sheet->nonDefaultColumnFormat(i)->setFiltered(m_undoData[i]);
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    m_sheet->cellStorage()->setDatabase(*this, Database());
    m_sheet->cellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void ApplyFilterCommand::setDatabase(const Database& database)
{
    m_database = database;
}

void ApplyFilterCommand::setOldFilter(const Filter& filter)
{
    m_oldFilter = new Filter(filter);
}
