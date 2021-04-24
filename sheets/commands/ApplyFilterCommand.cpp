/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ApplyFilterCommand.h"

#include <KLocalizedString>

#include "CellStorage.h"
#include "Damages.h"
#include "Map.h"
#include "Sheet.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"

#include "database/Database.h"
#include "database/Filter.h"

using namespace Calligra::Sheets;

ApplyFilterCommand::ApplyFilterCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Apply Filter"));
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
//         debugSheets <<"Filtering column/row" << i <<"?" << isFiltered;
        if (database.orientation() == Qt::Vertical) {
            m_undoData[i] = sheet->rowFormats()->isFiltered(i);
            sheet->rowFormats()->setFiltered(i, i, isFiltered);
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
            sheet->rowFormats()->setFiltered(i, i, m_undoData[i]);
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
