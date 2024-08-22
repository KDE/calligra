// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#include "DataManipulators.h"

#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/Formula.h"
#include "engine/ValueCalc.h"

#include <float.h>

using namespace Calligra::Sheets;

AbstractDataManipulator::AbstractDataManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
{
    m_checkLock = true;
}

AbstractDataManipulator::~AbstractDataManipulator() = default;

bool AbstractDataManipulator::process(Element *element)
{
    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col)
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Value val;
            //       int colidx = col - range.left();
            //       int rowidx = row - range.top();
            bool parse = false;
            Format::Type fmtType = Format::None;

            // do nothing if we don't want a change here
            if (!wantChange(element, col, row))
                continue;

            val = newValue(element, col, row, &parse, &fmtType);

            Cell cell = Cell(m_sheet, col, row);
            if (cell.isPartOfMerged())
                cell = cell.masterCell();

            // we have the data - set it !
            if (parse) {
                if (fmtType != Format::None) {
                    Style style;
                    style.setFormatType(fmtType);
                    cell.setStyle(style);
                }
                cell.parseUserInput(val.asString());
            } else {
                cell.setCellValue(val); // val can be empty - that's fine
                if (fmtType != Format::None) {
                    Style style;
                    style.setFormatType(fmtType);
                    cell.setStyle(style);
                }
            }
        }
    return true;
}

AbstractDFManipulator::AbstractDFManipulator(KUndo2Command *parent)
    : AbstractDataManipulator(parent)
{
    m_changeformat = true;
}

AbstractDFManipulator::~AbstractDFManipulator() = default;

bool AbstractDFManipulator::process(Element *element)
{
    // let parent class process it first
    AbstractDataManipulator::process(element);

    // don't continue if we don't have to change formatting
    if (!m_changeformat)
        return true;

    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col) {
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell(m_sheet, col, row);
            //       int colidx = col - range.left();
            //       int rowidx = row - range.top();
            Style style = newFormat(element, col, row);
            cell.setStyle(style);
        }
    }
    return true;
}

DataManipulator::DataManipulator(KUndo2Command *parent)
    : AbstractDataManipulator(parent)
    , m_format(Format::None)
    , m_parsing(false)
    , m_expandMatrix(false)
{
    // default name for DataManipulator, can be changed using setText
    setText(kundo2_i18n("Change Value"));
}

DataManipulator::~DataManipulator() = default;

bool DataManipulator::preProcess()
{
    // extend a singular region to the matrix size, if applicable
    if (m_firstrun && m_parsing && m_expandMatrix && Region::isSingular()) {
        const QString expression = m_data.asString();
        if (!expression.isEmpty() && expression[0] == '=') {
            Formula formula(m_sheet);
            formula.setExpression(expression);
            if (formula.isValid()) {
                const Value result = formula.eval();
                if (result.columns() > 1 || result.rows() > 1) {
                    const QPoint point = cells()[0]->rect().topLeft();
                    Region::add(QRect(point.x(), point.y(), result.columns(), result.rows()), m_sheet);
                }
            }
        } else if (!m_data.isArray()) {
            // not a formula; not a matrix: unset m_expandMatrix
            m_expandMatrix = false;
        }
    }
    return AbstractDataManipulator::preProcess();
}

bool DataManipulator::process(Element *element)
{
    bool success = AbstractDataManipulator::process(element);
    if (!success)
        return false;
    // Only lock cells, if expansion is desired and the value is a formula.
    if (m_expandMatrix && (m_data.asString().isEmpty() || m_data.asString().at(0) == '='))
        m_sheet->cellStorage()->lockCells(element->rect());
    return true;
}

bool DataManipulator::wantChange(Element *element, int col, int row)
{
    if (m_expandMatrix) {
        QRect range = element->rect();
        int colidx = col - range.left();
        int rowidx = row - range.top();
        // don't set this value, RecalcManager already did it
        if (colidx || rowidx)
            return false;
    }
    return true;
}

Value DataManipulator::newValue(Element *element, int col, int row, bool *parsing, Format::Type *formatType)
{
    *parsing = m_parsing;
    if (m_format != Format::None)
        *formatType = m_format;
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    return m_data.element(colidx, rowidx);
}

ShiftManipulator::ShiftManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_mode(Insert)
{
    m_checkLock = true;
    setText(kundo2_i18n("Insert Cells"));
}

ShiftManipulator::~ShiftManipulator() = default;

void ShiftManipulator::setRemove(bool remove)
{
    m_mode = remove ? Delete : Insert;
    if (!remove)
        setText(kundo2_i18n("Insert Cells"));
    else
        setText(kundo2_i18n("Remove Cells"));
}

bool ShiftManipulator::process(Element *element)
{
    const QRect range = element->rect();
    if (m_mode == Insert) { // insertion
        if (m_direction == ShiftBottom) {
            m_sheet->insertShiftDown(range); // this updates the cell refs
            m_sheet->cellStorage()->insertShiftDown(range);
        } else if (m_direction == ShiftRight) {
            m_sheet->insertShiftRight(range);
            m_sheet->cellStorage()->insertShiftRight(range);
        }
    } else { // deletion
        if (m_direction == ShiftBottom) {
            m_sheet->removeShiftUp(range);
            m_sheet->cellStorage()->removeShiftUp(range);
        } else if (m_direction == ShiftRight) {
            m_sheet->removeShiftLeft(range);
            m_sheet->cellStorage()->removeShiftLeft(range);
        }
    }
    const Region region(1, 1, KS_colMax, KS_rowMax, m_sheet);
    m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
    return true;
}

bool ShiftManipulator::undoNonCommandActions()
{
    Mode orig_mode = m_mode;
    // Revert the mode and run the commands. This will get the columns to where they need to be.
    // Then, the parent class will call undo actions to restore the contents.
    m_mode = (m_mode == Insert) ? Delete : Insert;
    performCommands();
    m_mode = orig_mode;
    return true;
}
