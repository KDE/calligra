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

#include "DeleteCommand.h"

#include "CellStorage.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "Validity.h"

#include <klocale.h>

using namespace KSpread;

DeleteCommand::DeleteCommand(QUndoCommand *parent)
        : AbstractDataManipulator(parent)
        , m_mode(Everything)
{
    setText(i18n("Delete"));
    m_checkLock = true;
}

DeleteCommand::~DeleteCommand()
{
    qDeleteAll(m_columnFormats);
    qDeleteAll(m_rowFormats);
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

    const QRect range = element->rect();

    if (element->isColumn()) {
        // column-wise processing
        for (int col = range.left(); col <= range.right(); ++col) {
            Cell cell = m_sheet->cellStorage()->firstInColumn(col);
            while (!cell.isNull()) {
                m_sheet->cellStorage()->take(col, cell.row());
                cell = m_sheet->cellStorage()->nextInColumn(col, cell.row());
            }
            if (m_mode == OnlyCells) {
                continue;
            }

            const ColumnFormat* columnFormat = m_sheet->columnFormat(col);
            if (m_firstrun && !columnFormat->isDefault()) {
                ColumnFormat* oldColumnFormat = new ColumnFormat(*columnFormat);
                oldColumnFormat->setNext(0);
                oldColumnFormat->setPrevious(0);
                m_columnFormats.insert(oldColumnFormat);
            }
            m_sheet->deleteColumnFormat(col);
        }
    } else if (element->isRow()) {
        // row-wise processing
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = m_sheet->cellStorage()->firstInRow(row);
            while (!cell.isNull()) {
                m_sheet->cellStorage()->take(cell.column(), row);
                cell = m_sheet->cellStorage()->nextInRow(cell.column(), row);
            }
            if (m_mode == OnlyCells) {
                continue;
            }

            const RowFormat* rowFormat = m_sheet->rowFormat(row);
            if (m_firstrun && !rowFormat->isDefault()) {
                RowFormat* oldRowFormat = new RowFormat(*rowFormat);
                oldRowFormat->setNext(0);
                oldRowFormat->setPrevious(0);
                m_rowFormats.insert(oldRowFormat);
            }
            m_sheet->deleteRowFormat(row);
        }
    } else {
        // row-wise processing
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = m_sheet->cellStorage()->firstInRow(row);
            if (!cell.isNull() && cell.column() < range.left())
                cell = m_sheet->cellStorage()->nextInRow(range.left() - 1, row);
            while (!cell.isNull()) {
                if (cell.column() > range.right())
                    break;

                m_sheet->cellStorage()->take(cell.column(), row);
                cell = m_sheet->cellStorage()->nextInRow(cell.column(), row);
            }
        }
    }

    // the rect storages
    m_sheet->cellStorage()->setComment(Region(range, element->sheet()), QString());
    m_sheet->cellStorage()->setConditions(Region(range, element->sheet()), Conditions());
    Style style;
    style.setDefault();
    m_sheet->cellStorage()->setStyle(Region(range, element->sheet()), style);
    m_sheet->cellStorage()->setValidity(Region(range, element->sheet()), Validity());
    return true;
}

bool DeleteCommand::mainProcessing()
{
    if (m_reverse) {
        foreach(ColumnFormat* columnFormat, m_columnFormats) {
            m_sheet->insertColumnFormat(new ColumnFormat(*columnFormat));
        }
        foreach(RowFormat* rowFormat, m_rowFormats) {
            m_sheet->insertRowFormat(new RowFormat(*rowFormat));
        }
    }
    return AbstractDataManipulator::mainProcessing();
}
