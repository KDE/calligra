/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ApplyFilterCommand.h"

#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

ApplyFilterCommand::ApplyFilterCommand()
    : AbstractRegionCommand()
{
    setText(kundo2_i18n("Apply Filter"));
}

ApplyFilterCommand::~ApplyFilterCommand() = default;

void ApplyFilterCommand::redo()
{
    m_undoData.clear();
    Database database = m_database;

    Sheet *sheet = dynamic_cast<Sheet *>(database.range().lastSheet());
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
            m_undoData[i] = sheet->columnFormats()->isFiltered(i);
            sheet->columnFormats()->setFiltered(i, i, isFiltered);
        }
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    m_sheet->fullCellStorage()->setDatabase(*this, Database());
    m_sheet->fullCellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void ApplyFilterCommand::undo()
{
    Database database = m_database;
    database.setFilter(m_oldFilter);

    Sheet *sheet = dynamic_cast<Sheet *>(database.range().lastSheet());
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database.orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i) {
        if (database.orientation() == Qt::Vertical)
            sheet->rowFormats()->setFiltered(i, i, m_undoData[i]);
        else // database.orientation() == Qt::Horizontal
            sheet->columnFormats()->setFiltered(i, i, m_undoData[i]);
    }
    if (database.orientation() == Qt::Vertical)
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::RowsChanged));
    else // database.orientation() == Qt::Horizontal
        sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::ColumnsChanged));

    m_sheet->fullCellStorage()->setDatabase(*this, Database());
    m_sheet->fullCellStorage()->setDatabase(*this, database);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
}

void ApplyFilterCommand::setDatabase(const Database &database)
{
    m_database = database;
}

void ApplyFilterCommand::setOldFilter(const Filter &filter)
{
    m_oldFilter = filter;
}
