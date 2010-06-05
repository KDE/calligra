/* This file is part of the KDE project
   Copyright (C) 2006 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "DataManipulators.h"

#include <klocale.h>

#include "Cell.h"
#include "CellStorage.h"
#include "CellStorage_p.h"
#include "Damages.h"
#include "Map.h"
#include "Sheet.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <float.h>
#include <math.h>

using namespace KSpread;

AbstractDataManipulator::AbstractDataManipulator(QUndoCommand* parent)
        : AbstractRegionCommand(parent)
{
    m_checkLock = true;
}

AbstractDataManipulator::~AbstractDataManipulator()
{
}

bool AbstractDataManipulator::process(Element* element)
{
    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col)
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Value val;
            QString text;
//       int colidx = col - range.left();
//       int rowidx = row - range.top();
            bool parse = false;
            Format::Type fmtType = Format::None;

            // do nothing if we don't want a change here
            if (!wantChange(element, col, row))
                continue;

            val = newValue(element, col, row, &parse, &fmtType);

            Cell cell = Cell(m_sheet, col, row);
            if (cell.isPartOfMerged()) cell = cell.masterCell();

            // we have the data - set it !
            if (parse) {
                if (fmtType != Format::None) {
                    Style style;
                    style.setFormatType(fmtType);
                    cell.setStyle(style);
                }
                cell.parseUserInput(val.asString());
            } else {
                if (!val.isEmpty()) {
                    cell.setValue(val);
                    cell.setUserInput(m_sheet->map()->converter()->asString(val).asString());
                    if (fmtType != Format::None) {
                        Style style;
                        style.setFormatType(fmtType);
                        cell.setStyle(style);
                    }
                }
            }
        }
    return true;
}

bool AbstractDataManipulator::preProcessing()
{
    // not the first run - data already stored ...
    if (!m_firstrun)
        return true;
    m_sheet->cellStorage()->startUndoRecording();
    return AbstractRegionCommand::preProcessing();
}

bool AbstractDataManipulator::mainProcessing()
{
    if (m_reverse) {
        // reverse - use the stored value
        QUndoCommand::undo(); // undo child commands
        return true;
    }
    return AbstractRegionCommand::mainProcessing();
}

bool AbstractDataManipulator::postProcessing()
{
    // not the first run - data already stored ...
    if (!m_firstrun)
        return true;
    m_sheet->cellStorage()->stopUndoRecording(this);
    return true;
}

AbstractDFManipulator::AbstractDFManipulator()
{
    m_changeformat = true;
}

AbstractDFManipulator::~AbstractDFManipulator()
{
}

bool AbstractDFManipulator::process(Element* element)
{
    // let parent class process it first
    AbstractDataManipulator::process(element);

    // don't continue if we don't have to change formatting
    if (!m_changeformat) return true;
    if (m_reverse) return true; // undo done by AbstractDataManipulator

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


DataManipulator::DataManipulator(QUndoCommand* parent)
        : AbstractDataManipulator(parent)
        , m_format(Format::None)
        , m_parsing(false)
        , m_expandMatrix(false)
{
    // default name for DataManipulator, can be changed using setText
    setText(i18n("Change Value"));
}

DataManipulator::~DataManipulator()
{
}

bool DataManipulator::preProcessing()
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
    return AbstractDataManipulator::preProcessing();
}

bool DataManipulator::process(Element* element)
{
    bool success = AbstractDataManipulator::process(element);
    if (!success)
        return false;
    if (!m_reverse) {
        // Only lock cells, if expansion is desired and the value is a formula.
        if (m_expandMatrix && (m_data.asString().isEmpty() || m_data.asString().at(0) == '='))
            m_sheet->cellStorage()->lockCells(element->rect());
    }
    return true;
}

Value DataManipulator::newValue(Element *element, int col, int row,
                                bool *parsing, Format::Type *formatType)
{
    *parsing = m_parsing;
    if (m_format != Format::None)
        *formatType = m_format;
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if (m_parsing && m_expandMatrix) {
        if (colidx || rowidx) {
            *parsing = false;
            if (m_data.asString().isEmpty() || m_data.asString().at(0) == '=')
                m_sheet->cellStorage()->setValue(col, row, Value()); // for proper undo
            return Cell(m_sheet, range.topLeft()).value().element(colidx, rowidx);
        }
    }
    return m_data.element(colidx, rowidx);
}


SeriesManipulator::SeriesManipulator()
{
    setText(i18n("Insert Series"));

    m_type = Linear;
    m_last = -2;
}

SeriesManipulator::~SeriesManipulator()
{
}

void SeriesManipulator::setupSeries(const QPoint &_marker, double start,
                                    double end, double step, Series mode, Series type)
{
    m_type = type;
    m_start = Value(start);
    m_step = Value(step);
    // compute cell count
    int numberOfCells = 1;
    if (type == Linear)
        numberOfCells = (int)((end - start) / step + 1);
    if (type == Geometric)
        /* basically, A(n) = start * step ^ n
        * so when is end >= start * step ^ n ??
        * when n = ln(end/start) / ln(step)
        */
        // DBL_EPSILON is added to prevent rounding errors
        numberOfCells = (int)(::log(end / start) / ::log(step) + DBL_EPSILON) + 1;

    // with this, generate range information
    Region range(_marker.x(), _marker.y(), (mode == Column) ? 1 : numberOfCells,
                 (mode == Row) ? 1 : numberOfCells);

    // and add the range to the manipulator
    add(range);
}

Value SeriesManipulator::newValue(Element *element, int col, int row,
                                  bool *parse, Format::Type *)
{
    *parse = false;
    ValueCalc *calc = m_sheet->map()->calc();

    // either colidx or rowidx is always zero
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    int which = (colidx > 0) ? colidx : rowidx;
    Value val;
    if (which == m_last + 1) {
        // if we are requesting next item in the series, which should almost always
        // be the case, we can use the pre-computed value to speed up the process
        if (m_type == Linear)
            val = calc->add(m_prev, m_step);
        if (m_type == Geometric)
            val = calc->mul(m_prev, m_step);
    } else {
        // otherwise compute from scratch
        val = m_start;
        for (int i = 0; i < which; ++i) {
            if (m_type == Linear)
                val = calc->add(val, m_step);
            if (m_type == Geometric)
                val = calc->mul(val, m_step);
        }
    }
    // store last value
    m_prev = val;
    m_last = which;

    // return the computed value
    return val;
}


FillManipulator::FillManipulator()
{
    m_dir = Down;
    m_changeformat = true;
    setText(i18n("Fill Selection"));
}

FillManipulator::~FillManipulator()
{
}

Value FillManipulator::newValue(Element *element, int col, int row,
                                bool *parse, Format::Type *fmtType)
{
    Q_UNUSED(fmtType);
    const int targetRow = row;
    const int targetCol = col;
    switch (m_dir) {
    case Up:    row = element->rect().bottom(); break;
    case Down:  row = element->rect().top();    break;
    case Left:  col = element->rect().right();  break;
    case Right: col = element->rect().left();   break;
    };
    Cell cell(m_sheet, col, row); // the reference cell
    if (cell.isFormula()) {
        *parse = true;
        return Value(Cell(m_sheet, targetCol, targetRow).decodeFormula(cell.encodeFormula()));
    }
    return cell.value();
}

Style FillManipulator::newFormat(Element *element, int col, int row)
{
    switch (m_dir) {
    case Up:    row = element->rect().bottom(); break;
    case Down:  row = element->rect().top();    break;
    case Left:  col = element->rect().right();  break;
    case Right: col = element->rect().left();   break;
    };
    return Cell(m_sheet, col, row).style();
}

CaseManipulator::CaseManipulator()
{
    m_mode = Upper;
    setText(i18n("Change Case"));
}

CaseManipulator::~CaseManipulator()
{
}

Value CaseManipulator::newValue(Element *element, int col, int row,
                                bool *parse, Format::Type *)
{
    Q_UNUSED(element)
    // if we are here, we know that we want the change
    *parse = false;
    QString str = Cell(m_sheet, col, row).value().asString();
    switch (m_mode) {
    case Upper: str = str.toUpper();
        break;
    case Lower: str = str.toLower();
        break;
    case FirstUpper:
        if (str.length() > 0)
            str = str.at(0).toUpper() + str.right(str.length() - 1);
        break;
    };
    return Value(str);
}

bool CaseManipulator::wantChange(Element *element, int col, int row)
{
    Q_UNUSED(element)
    Cell cell(m_sheet, col, row);
    // don't change cells with a formula
    if (cell.isFormula())
        return false;
    // don't change cells containing other things than strings
    if (!cell.value().isString())
        return false;
    // original version was dismissing text starting with '!' and '*', is this
    // necessary ?
    return true;
}



ShiftManipulator::ShiftManipulator(QUndoCommand *parent)
        : AbstractRegionCommand(parent)
        , m_mode(Insert)
{
    m_checkLock = true;
    setText(i18n("Insert Cells"));
}

ShiftManipulator::~ShiftManipulator()
{
}

void ShiftManipulator::setReverse(bool reverse)
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
    if (!m_reverse)
        setText(i18n("Insert Cells"));
    else
        setText(i18n("Remove Cells"));
}

bool ShiftManipulator::process(Element* element)
{
    // create undo for styles, comments, conditions, validity
    if (m_firstrun)
        m_sheet->cellStorage()->startUndoRecording();

    const QRect range = element->rect();
    if (!m_reverse) { // insertion
        if (m_direction == ShiftBottom) {
            m_sheet->insertShiftDown(range);
            m_sheet->cellStorage()->insertShiftDown(range);
        } else if (m_direction == ShiftRight) {
            m_sheet->insertShiftRight(range);
            m_sheet->cellStorage()->insertShiftRight(range);
        }

        // undo deletion
        if (m_mode == Delete) {
            QUndoCommand::undo(); // undo child commands
        }
    } else { // deletion
        if (m_direction == ShiftBottom) {
            m_sheet->removeShiftUp(range);
            m_sheet->cellStorage()->removeShiftUp(range);
        } else if (m_direction == ShiftRight) {
            m_sheet->removeShiftLeft(range);
            m_sheet->cellStorage()->removeShiftLeft(range);
        }

        // undo insertion
        if (m_mode == Insert) {
            QUndoCommand::undo(); // undo child commands
        }
    }

    if (m_firstrun)
        m_sheet->cellStorage()->stopUndoRecording(this);
    return true;
}

bool ShiftManipulator::postProcessing()
{
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
    return true;
}
