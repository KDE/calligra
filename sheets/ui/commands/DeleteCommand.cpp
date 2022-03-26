/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DeleteCommand.h"

#include "engine/Validity.h"
#include "core/CellStorage.h"
#include "core/Condition.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

DeleteCommand::DeleteCommand(KUndo2Command *parent)
        : AbstractDataManipulator(parent)
        , m_mode(Everything)
{
    setText(kundo2_i18n("Delete"));
    m_checkLock = true;
}

DeleteCommand::~DeleteCommand()
{
}

void DeleteCommand::setMode(Mode mode)
{
    m_mode = mode;
}

bool DeleteCommand::process(Element* element)
{
    Q_ASSERT(!m_reverse);

    // The RecalcManager needs a valid sheet.
    if (!element->sheet())
        element->setSheet(m_sheet);

    CellStorage *cs = m_sheet->fullCellStorage();
    ColFormatStorage *cols = m_sheet->columnFormats();
    RowFormatStorage *rows = m_sheet->rowFormats();
    const QRect range = element->rect();

    if (element->isColumn()) {
        // column-wise processing
        for (int col = range.left(); col <= range.right(); ++col) {
            Cell cell = cs->firstInColumn(col);
            while (!cell.isNull()) {
                cs->take(col, cell.row());
                cell = cs->nextInColumn(col, cell.row());
            }
            if (m_mode == OnlyCells)
                continue;

            if (m_firstrun && (!cols->isDefaultCol(col))) {
                ColFormat cf = cols->getColFormat(col);
                m_columnFormats[col] = cf;
            }
            m_sheet->clearColumnFormat(col);
        }
    } else if (element->isRow()) {
        // row-wise processing
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = cs->firstInRow(row);
            while (!cell.isNull()) {
                cs->take(cell.column(), row);
                cell = cs->nextInRow(cell.column(), row);
            }
            if (m_mode == OnlyCells) {
                continue;
            }
            if (m_firstrun && (!rows->isDefaultRow(row))) {
                RowFormat rf = rows->getRowFormat(row);
                m_rowFormats[row] = rf;
            }
            m_sheet->clearRowFormat(row);
        }
    } else {
        // row-wise processing
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = cs->firstInRow(row);
            if (!cell.isNull() && cell.column() < range.left())
                cell = cs->nextInRow(range.left() - 1, row);
            while (!cell.isNull()) {
                if (cell.column() > range.right())
                    break;

                cs->take(cell.column(), row);
                cell = cs->nextInRow(cell.column(), row);
            }
        }
    }

    // the rect storages
    cs->setComment(Region(range, element->sheet()), QString());
    cs->setConditions(Region(range, element->sheet()), Conditions());
    Style style;
    style.setDefault();
    cs->setStyle(Region(range, element->sheet()), style);
    cs->setValidity(Region(range, element->sheet()), Validity());
    return true;
}

bool DeleteCommand::mainProcessing()
{
    if (m_reverse) {
        for (int col : m_columnFormats.keys()) {
            m_sheet->columnFormats()->setColFormat(col, col, m_columnFormats.value(col));
        }
        for (int row : m_rowFormats.keys()) {
            m_sheet->rowFormats()->setRowFormat(row, row, m_rowFormats.value(row));
        }
    }
    return AbstractDataManipulator::mainProcessing();
}

