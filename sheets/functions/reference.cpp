// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in reference functions
#include "ReferenceModule.h"

#include "engine/CalculationSettings.h"
#include "engine/CellBase.h"
#include "engine/CellBaseStorage.h"
#include "engine/Formula.h"
#include "engine/Function.h"
#include "engine/MapBase.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

#include <QRegularExpression>
#include <algorithm>

using namespace Calligra::Sheets;
using namespace Qt::StringLiterals;

// prototypes (sorted alphabetically)
Value func_address(valVector args, ValueCalc *calc, FuncExtra *);
Value func_areas(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cell(valVector args, ValueCalc *calc, FuncExtra *e);
Value func_choose(valVector args, ValueCalc *calc, FuncExtra *);
Value func_column(valVector args, ValueCalc *calc, FuncExtra *);
Value func_columns(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dde(valVector args, ValueCalc *calc, FuncExtra *);
Value func_hlookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_hyperlink(valVector args, ValueCalc *calc, FuncExtra *);
Value func_index(valVector args, ValueCalc *calc, FuncExtra *);
Value func_indirect(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_xlookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_match(valVector args, ValueCalc *calc, FuncExtra *);
Value func_multiple_operations(valVector args, ValueCalc *calc, FuncExtra *);
Value func_offset(valVector args, ValueCalc *calc, FuncExtra *);
Value func_row(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rows(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sheet(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sheets(valVector args, ValueCalc *calc, FuncExtra *);
Value func_vlookup(valVector args, ValueCalc *calc, FuncExtra *);

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreadreferencemodule.json", ReferenceModule)

ReferenceModule::ReferenceModule(QObject *parent, const QVariantList &)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("ADDRESS", func_address);
    f->setParamCount(2, 5);
    add(f);
    f = new Function("AREAS", func_areas);
    f->setParamCount(1);
    f->setNeedsExtra(true);
    f->setAcceptArray();
    add(f);
    f = new Function("CELL", func_cell);
    f->setParamCount(1, 2);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("CHOOSE", func_choose);
    f->setParamCount(2, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("COLUMN", func_column);
    f->setParamCount(0, 1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("COLUMNS", func_columns);
    f->setParamCount(1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("DDE", func_dde);
    f->setParamCount(3, 4);
    add(f);
    f = new Function("HLOOKUP", func_hlookup);
    f->setParamCount(3, 4);
    f->setAcceptArray();
    add(f);
    f = new Function("HYPERLINK", func_hyperlink);
    f->setParamCount(1, 2);
    add(f);
    f = new Function("INDEX", func_index);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("INDIRECT", func_indirect);
    f->setParamCount(1, 2);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("LOOKUP", func_lookup);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("XLOOKUP", func_xlookup);
    f->setParamCount(3, 6);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("MATCH", func_match);
    f->setParamCount(2, 3);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("MULTIPLE.OPERATIONS", func_multiple_operations);
    f->setParamCount(3, 5);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("OFFSET", func_offset);
    f->setParamCount(3, 5);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("ROW", func_row);
    f->setParamCount(0, 1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("ROWS", func_rows);
    f->setParamCount(1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("SHEET", func_sheet);
    f->setParamCount(0, 1);
    f->setNeedsExtra(true);
    add(f);
    f = new Function("SHEETS", func_sheets);
    f->setParamCount(0, 1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("VLOOKUP", func_vlookup);
    f->setParamCount(3, 4);
    f->setAcceptArray();
    add(f);
}

QString ReferenceModule::descriptionFileName() const
{
    return QString("reference.xml");
}

//
// Function: ADDRESS
//
Value func_address(valVector args, ValueCalc *calc, FuncExtra *)
{
    bool r1c1 = false;
    QString sheetName;
    int absNum = 1;
    if (args.count() > 2) {
        absNum = calc->conv()->asInteger(args[2]).asInteger();
    }
    if (absNum < 1 || absNum > 4) {
        return Value::errorVALUE();
    }
    if (args.count() > 3 && !args[3].isEmpty()) {
        r1c1 = !(calc->conv()->asBoolean(args[3]).asBoolean());
    }
    if (args.count() == 5) {
        sheetName = calc->conv()->asString(args[4]).asString();
    }

    QString result;
    int row = calc->conv()->asInteger(args[0]).asInteger();
    int col = calc->conv()->asInteger(args[1]).asInteger();
    if (row < 1 || col < 1) {
        return Value::errorVALUE();
    }

    if (!sheetName.isEmpty()) {
        if (sheetName.contains(u' ')) {
            result += '\'';
            result += sheetName;
            result += '\'';
        } else {
            result += sheetName;
        }
        result += r1c1 ? '!' : '.';
    }

    if (r1c1) {
        // row first
        bool abs = false;
        if (absNum == 1 || absNum == 2)
            abs = true;

        result += 'R';
        if (!abs)
            result += '[';
        result += QString::number(row);

        if (!abs)
            result += ']';

        // column
        abs = false;
        if (absNum == 1 || absNum == 3)
            abs = true;

        result += 'C';
        if (!abs)
            result += '[';
        result += QString::number(col);

        if (!abs)
            result += ']';
    } else {
        bool abs = false;
        if (absNum == 1 || absNum == 3)
            abs = true;

        if (abs)
            result += '$';

        result += CellBase::columnName(col);

        abs = false;
        if (absNum == 1 || absNum == 2)
            abs = true;

        if (abs)
            result += '$';

        result += QString::number(row);
    }

    return Value(result);
}

//
// Function: AREAS
//
Value func_areas(valVector args, ValueCalc *calc, FuncExtra *e)
{
    if (e) {
        if (e->regions[0].isValid())
            return Value(e->regions[0].rects().size());

        if ((e->ranges[0].col1 != -1) && (e->ranges[0].row1 != -1) && (e->ranges[0].col2 != -1) && (e->ranges[0].row2 != -1))
            // we have a range reference - return 1
            return Value(1);
    }

    QString s = calc->conv()->asString(args[0]).asString();
    if (s[0] != '(' || s[s.length() - 1] != ')')
        return Value::errorVALUE();

    int l = s.length();

    int num = 0;
    QString ref;
    for (int i = 1; i < l; ++i) {
        if (s[i] == ',' || s[i] == ')') {
            Calligra::Sheets::Region region = e->sheet->map()->regionFromName(ref, e->sheet);
            if (!region.isValid())
                return Value::errorVALUE();
            else {
                ++num;
                ref.clear();
            }
        } else
            ref += s[i];
    }

    return Value(num);
}

//
// Function: CELL
//
Value func_cell(valVector args, ValueCalc *calc, FuncExtra *e)
{
    const QString type = calc->conv()->asString(args[0]).asString().toLower();

    args.pop_front();
    FuncExtra extra(*e);
    extra.ranges.pop_front();
    extra.regions.pop_front();

    if (type == "col") {
        return func_column(args, calc, &extra);
    }
    if (type == "row") {
        return func_row(args, calc, &extra);
    }
    if (type == "sheet") {
        return func_sheet(args, calc, &extra);
    }
    if (type == "address") {
        const Calligra::Sheets::Region &region = args.count() ? extra.regions[0] : Calligra::Sheets::Region(QPoint(e->mycol, e->myrow), e->sheet);
        QString s;
        if (region.firstSheet() && region.firstSheet() != e->sheet)
            s += '\'' + region.firstSheet()->sheetName() + "'!";
        s += '$' + CellBase::columnName(region.firstRange().x()) + '$' + QString::number(region.firstRange().y());
        if (region.firstRange() != region.lastRange())
            s += ":$" + CellBase::columnName(region.lastRange().x()) + '$' + QString::number(region.lastRange().y());
        return Value(s);
    }
    if (type == "filename") {
        return Value(calc->settings()->fileName());
    }

    /*TODO
    if (type == "contents")
    if (type == "color")
    if (type == "format")
    if (type == "width")
    if (type == "type")
    if (type == "protect")
    if (type == "parenthesis")
    if (type == "prefix")
    */

    return Value::errorVALUE();
}

//
// Function: CHOOSE
//
Value func_choose(valVector args, ValueCalc *calc, FuncExtra *)
{
    int cnt = args.count() - 1;
    int num = calc->conv()->asInteger(args[0]).asInteger();
    if ((num <= 0) || (num > cnt))
        return Value::errorVALUE();
    return args[num];
}

//
// Function: COLUMN
//
Value func_column(valVector args, ValueCalc *, FuncExtra *e)
{
    int col = e ? e->mycol : 0;
    if (e && args.count() && e->regions.size() && e->regions[0].isValid()) {
        const QRect rect = e->regions[0].firstRange();
        if (rect.width() > 1) {
            Value result(Value::Array);
            for (int i = 0; i < rect.width(); ++i)
                result.setElement(i, 0, Value(rect.left() + i));
            return result;
        }
    }
    if (e) {
        const auto matrix = e->sheet->cellStorage()->matrixStorage()->containedPair(QPoint(e->mycol, e->myrow));
        if (matrix.second && (matrix.first.width() > 1 || matrix.first.height() > 1)) {
            Value result(Value::Array);
            const int left = matrix.first.left();
            for (int row = 0; row < matrix.first.height(); ++row) {
                for (int i = 0; i < matrix.first.width(); ++i) {
                    result.setElement(i, row, Value(left + i));
                }
            }
            return result;
        }
    }
    if (e && args.count() && e->ranges.size() && e->ranges[0].col1 >= 0) {
        auto range = e->ranges[0];
        const int width = range.columns();
        if (width > 1) {
            Value result(Value::Array);
            for (int i = 0; i < width; ++i)
                result.setElement(i, 0, Value(range.col1 + i));
            return result;
        }
    }
    if (e && args.count())
        col = e->ranges[0].col1;
    if (col > 0)
        return Value(col);
    return Value::errorVALUE();
}

//
// Function: COLUMNS
//
Value func_columns(valVector, ValueCalc *, FuncExtra *e)
{
    int col1 = e->ranges[0].col1;
    int col2 = e->ranges[0].col2;
    if ((col1 == -1) || (col2 == -1))
        return Value::errorVALUE();
    return Value(col2 - col1 + 1);
}

//
// Function: DDE
//
// Requests data from another running application via Dynamic Data Exchange, a Windows-only
// inter-process mechanism this platform has no equivalent of. There is nothing meaningful to
// evaluate DDE(...) to, so it always yields an error -- registering it (rather than failing to
// parse the formula at all) is what OpenFormula compliance requires for documents that use it.
//
Value func_dde(valVector, ValueCalc *, FuncExtra *)
{
    return Value::errorNA();
}

//
// Function: HYPERLINK
//
// Returns the display text for a hyperlink (or the URL itself, if no display text was given).
// Following the link is a UI concern handled outside formula evaluation.
//
Value func_hyperlink(valVector args, ValueCalc *calc, FuncExtra *)
{
    Q_UNUSED(calc);
    if (args[0].isError()) {
        return args[0];
    }
    if (args.count() > 1) {
        if (args[1].isError()) {
            return args[1];
        }
        if (args[1].isEmpty()) {
            return Value(0);
        }
        return args[1];
    }
    return args[0];
}

//
// Function: HLOOKUP
//
Value func_hlookup(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value key = args[0];
    const Value data = args[1];
    const int row = calc->conv()->asInteger(args[2]).asInteger();
    const int cols = data.columns();
    const int rows = data.rows();
    if (row < 1 || row > rows)
        return Value::errorVALUE();
    const bool rangeLookup = (args.count() > 3) ? calc->conv()->asBoolean(args[3]).asBoolean() : true;

    // now traverse the array and perform comparison
    Value r;
    Value v = Value::errorNA();
    for (int col = 0; col < cols; ++col) {
        // search in the first row
        const Value le = data.element(col, 0);
        if (calc->naturalEqual(key, le)) {
            return data.element(col, row - 1);
        }
        // optionally look for the next largest value that is less than key
        const bool comparable = (key.isString() == le.isString()) || (key.isNumber() && le.isNumber());
        if (rangeLookup && comparable && calc->naturalLower(le, key) && calc->naturalLower(r, le)) {
            r = le;
            v = data.element(col, row - 1);
        }
    }
    return v;
}

//
// Function: INDEX
//
Value func_index(valVector args, ValueCalc *calc, FuncExtra *)
{
    // first argument can be either a range, then we return a given cell's
    // value, or a single cell containing an array - then we return the array
    // element. In any case, this function can assume that the given value
    // is the same. Because it is.

    Value val = args[0];
    const int requestedRow = calc->conv()->asInteger(args[1]).asInteger();
    const int requestedCol = calc->conv()->asInteger(args[2]).asInteger();
    if (requestedRow < 0 || requestedCol < 0 || requestedRow > (int)val.rows() || requestedCol > (int)val.columns()) {
        return Value::errorREF();
    }
    if (requestedRow == 0 && requestedCol == 0) {
        return val;
    }
    if (requestedRow == 0) {
        Value result(Value::Array);
        for (unsigned col = 0; col < val.columns(); ++col) {
            result.setElement(col, 0, val.element(col, requestedCol - 1));
        }
        return result;
    }
    if (requestedCol == 0) {
        Value result(Value::Array);
        for (unsigned row = 0; row < val.rows(); ++row) {
            result.setElement(0, row, val.element(row, requestedRow - 1));
        }
        return result;
    }
    return val.element(requestedCol - 1, requestedRow - 1);
}

//
// Function: INDIRECT
//
Value func_indirect(valVector args, ValueCalc *calc, FuncExtra *e)
{
    bool r1c1 = false;
    QString ref = calc->conv()->asString(args[0]).asString();
    if (args.count() == 2)
        r1c1 = !(calc->conv()->asBoolean(args[1]).asBoolean());

    if (ref.isEmpty())
        return Value::errorVALUE();

    if (r1c1) {
        const QRegularExpression match(u"^R(\\d+)C(\\d+)$"_s, QRegularExpression::CaseInsensitiveOption);
        const auto result = match.match(ref);
        if (!result.hasMatch()) {
            return Value::errorREF();
        }
        int column = result.captured(2).toInt();
        QString columnName;
        while (column > 0) {
            const int digit = (column - 1) % 26;
            columnName.prepend(QChar('A' + digit));
            column = (column - 1) / 26;
        }
        ref = columnName + result.captured(1);
    }

    const Calligra::Sheets::Region region = e->sheet->map()->regionFromName(ref, e->sheet);
    if (!region.isValid() || !region.isSingular())
        return Value::errorVALUE();

    const CellBase cell(region.firstSheet(), region.firstRange().topLeft());
    if (!cell.isNull())
        return cell.value();
    return Value::errorVALUE();
}

//
// Function: LOOKUP
//
Value func_xlookup(valVector args, ValueCalc *, FuncExtra *e)
{
    const Value lookupValue = args[0];
    Value lookupArray = args[1];
    Value returnArray = args[2];
    if (lookupArray.type() == Value::CellRange && e && !e->ranges.isEmpty()) {
        auto range = e->ranges[0];
        lookupArray = Value(Value::Array);
        for (int row = 0; row < range.rows(); ++row) {
            for (int col = 0; col < range.columns(); ++col) {
                lookupArray.setElement(col, row, CellBase(e->sheet, range.col1 + col, range.row1 + row).value());
            }
        }
    }
    if (returnArray.type() == Value::CellRange && e && e->ranges.size() > 1) {
        auto range = e->ranges[1];
        returnArray = Value(Value::Array);
        for (int row = 0; row < range.rows(); ++row) {
            for (int col = 0; col < range.columns(); ++col) {
                returnArray.setElement(col, row, CellBase(e->sheet, range.col1 + col, range.row1 + row).value());
            }
        }
    }
    for (unsigned row = 0; row < lookupArray.rows(); ++row) {
        for (unsigned col = 0; col < lookupArray.columns(); ++col) {
            if (lookupArray.element(col, row) == lookupValue) {
                const unsigned resultRow = returnArray.rows() == 1 ? 0 : row;
                const unsigned resultCol = returnArray.columns() == 1 ? 0 : col;
                return returnArray.element(resultCol, resultRow);
            }
        }
    }
    if (args.size() > 3 && !args[3].isEmpty()) {
        return args[3];
    }
    return Value::errorNA();
}

Value func_lookup(valVector args, ValueCalc *calc, FuncExtra *e)
{
    Value num = args[0];
    if (num.isArray()) {
        return Value::errorVALUE();
    }
    Value lookup = args[1];
    Value rr = args[2];
    auto materialize = [&](Value value, int rangeIndex) {
        if (value.type() != Value::CellRange || !e || rangeIndex >= e->ranges.size()) {
            return value;
        }
        auto range = e->ranges[rangeIndex];
        Value result(Value::Array);
        for (int row = 0; row < range.rows(); ++row) {
            for (int col = 0; col < range.columns(); ++col) {
                result.setElement(col, row, CellBase(e->sheet, range.col1 + col, range.row1 + row).value());
            }
        }
        return result;
    };
    lookup = materialize(lookup, 0);
    rr = materialize(rr, 1);
    unsigned cols = lookup.columns();
    unsigned rows = lookup.rows();
    if ((cols != rr.columns()) || (rows != rr.rows()))
        return Value::errorVALUE();
    Value res = Value::errorNA();

    // now traverse the array and perform comparison
    for (unsigned r = 0; r < rows; ++r)
        for (unsigned c = 0; c < cols; ++c) {
            // update the result, return if we cross the line
            Value le = lookup.element(c, r);
            if (calc->naturalEqual(le, num, calc->settings()->caseSensitiveComparisons())) {
                return rr.element(c, r);
            }
            if (calc->naturalLower(le, num, calc->settings()->caseSensitiveComparisons())) {
                res = rr.element(c, r);
            }
        }
    return res;
}

//
// Function: MATCH
//
Value func_match(valVector args, ValueCalc *calc, FuncExtra *e)
{
    int matchType = 1;
    if (args.count() == 3) {
        bool ok = true;
        matchType = calc->conv()->asInteger(args[2], &ok).asInteger();
        if (!ok)
            return Value::errorVALUE(); // invalid matchtype
    }

    const Value &searchValue = args[0];
    Value searchArray = args[1];

    if (searchValue.isArray()) {
        Value result(Value::Array);
        for (unsigned row = 0; row < searchValue.rows(); ++row) {
            for (unsigned col = 0; col < searchValue.columns(); ++col) {
                valVector scalarArgs = args;
                scalarArgs[0] = searchValue.element(col, row);
                result.setElement(col, row, func_match(scalarArgs, calc, e));
            }
        }
        return result;
    }

    // Range arguments are passed as their first cell value; materialize the
    // complete range so MATCH can inspect every candidate.
    if (e->ranges.size() > 1 && (e->ranges[1].rows() > 1 || e->ranges[1].columns() > 1)) {
        auto range = e->ranges[1];
        searchArray = Value(Value::Array);
        for (int row = 0; row < range.rows(); ++row) {
            for (int col = 0; col < range.columns(); ++col) {
                searchArray.setElement(col, row, CellBase(e->sheet, range.col1 + col, range.row1 + row).value());
            }
        }
    }

    if (e->ranges[1].rows() != 1 && e->ranges[1].columns() != 1)
        return Value::errorNA();
    int dr = 1, dc = 0;
    if (searchArray.columns() != 1) {
        dr = 0;
        dc = 1;
    }
    int n = qMax(searchArray.rows(), searchArray.columns());

    if (matchType == 0) {
        for (int i = 0; i < n; ++i) {
            if (calc->naturalEqual(searchValue, searchArray.element(i * dc, i * dr), false)) {
                return Value(i + 1);
            }
        }
        return Value::errorNA();
    }
    int candidate = -1;
    for (int i = 0; i < n; ++i) {
        const Value item = searchArray.element(i * dc, i * dr);
        const bool comparable =
            (searchValue.isNumber() && item.isNumber()) || (searchValue.isString() && item.isString()) || (searchValue.isBoolean() && item.isBoolean());
        if (comparable && (matchType > 0 ? calc->naturalLequal(item, searchValue, false) : calc->naturalGequal(item, searchValue, false))) {
            candidate = i;
        }
    }
    return candidate < 0 ? Value::errorNA() : Value(candidate + 1);
}

//
// Function: MULTIPLE.OPERATIONS
//
Value func_multiple_operations(valVector args, ValueCalc *, FuncExtra *e)
{
    if (args.count() != 3 && args.count() != 5)
        return Value::errorVALUE(); // invalid number of parameters

    for (int i = 0; i < args.count(); i++) {
        if (e->ranges[i].col1 == -1 || e->ranges[i].row1 == -1)
            return Value::errorVALUE();
    }

    CellBaseStorage *s = e->sheet->cellStorage();

    // get formula to evaluate
    int formulaCol = e->ranges[0].col1;
    int formulaRow = e->ranges[0].row1;
    Formula formula = s->formula(formulaCol, formulaRow);
    if (!formula.isValid())
        return Value::errorVALUE();

    CellIndirection cellIndirections;
    cellIndirections.insert(CellBase(e->sheet, e->ranges[1].col1, e->ranges[1].row1), CellBase(e->sheet, e->ranges[2].col1, e->ranges[2].row1));
    if (args.count() > 3) {
        cellIndirections.insert(CellBase(e->sheet, e->ranges[3].col1, e->ranges[3].row1), CellBase(e->sheet, e->ranges[4].col1, e->ranges[4].row1));
    }

    return formula.eval(cellIndirections);
}

//
// Function: OFFSET
//
Value func_offset(valVector args, ValueCalc *calc, FuncExtra *e)
{
    const int rowPlus = calc->conv()->asInteger(args[1]).asInteger();
    const int colPlus = calc->conv()->asInteger(args[2]).asInteger();

    const int height = args.count() >= 4 ? calc->conv()->asInteger(args[3]).asInteger() : e->ranges[0].rows();
    const int width = args.count() >= 5 ? calc->conv()->asInteger(args[4]).asInteger() : e->ranges[0].columns();
    if (height <= 0 || width <= 0) {
        return Value::errorVALUE();
    }

    // Doesn't take references to other sheets into account
    // const QRect rect(e->ranges[0].col1, e->ranges[0].row1, e->ranges[0].col2, e->ranges[0].row2);
    // const Calligra::Sheets::Region region(rect, e->sheet);

    if (e->regions.isEmpty())
        return Value::errorVALUE();

    const Calligra::Sheets::Region &region = e->regions[0];

    if (!region.isValid() /* || !region.isSingular() */)
        return Value::errorVALUE();

    const QRect sourceRect = region.firstRange();
    const bool resizeRange = args.count() >= 4 || args.count() >= 5;
    const int resultHeight = resizeRange && sourceRect.height() > 1 ? sourceRect.height() : height;
    const int resultWidth = resizeRange && sourceRect.width() > 1 ? sourceRect.width() : width;

    QPoint p = region.firstRange().topLeft() + QPoint(colPlus, rowPlus);
    if (resultHeight == 1 && resultWidth == 1) {
        const CellBase cell(region.firstSheet(), p);
        return cell.isNull() ? Value::errorREF() : cell.value();
    }
    Value result(Value::Array);
    for (int row = 0; row < resultHeight; ++row) {
        for (int col = 0; col < resultWidth; ++col) {
            const CellBase cell(region.firstSheet(), p.x() + col, p.y() + row);
            result.setElement(col, row, cell.isNull() ? Value::empty() : cell.value());
        }
    }
    return result;
}

//
// Function: ROW
//
Value func_row(valVector args, ValueCalc *, FuncExtra *e)
{
    int row = e ? e->myrow : 0;
    if (e && args.count() && e->regions.size() && e->regions[0].isValid()) {
        const QRect rect = e->regions[0].firstRange();
        if (rect.height() > 1) {
            Value result(Value::Array);
            for (int i = 0; i < rect.height(); ++i) {
                result.setElement(0, i, Value(rect.top() + i));
            }
            return result;
        }
    }
    if (e && args.count() && args[0].isArray() && args[0].columns() == 1 && args[0].rows() > 1) {
        Value result(Value::Array);
        for (unsigned i = 0; i < args[0].rows(); ++i) {
            result.setElement(0, i, Value(e->myrow - 1 + static_cast<int>(i)));
        }
        return result;
    }
    if (e) {
        const auto matrix = e->sheet->cellStorage()->matrixStorage()->containedPair(QPoint(e->mycol, e->myrow));
        if (matrix.second && matrix.first.height() > 1) {
            Value result(Value::Array);
            const int top = matrix.first.top();
            for (int i = 0; i < matrix.first.height(); ++i) {
                result.setElement(0, i, Value(top + i - (args.isEmpty() ? 0 : 1)));
            }
            return result;
        }
    }
    if (e && args.count() && e->ranges.size() && e->ranges[0].row1 >= 0) {
        auto range = e->ranges[0];
        const int height = range.rows();
        if (height > 1) {
            Value result(Value::Array);
            for (int i = 0; i < height; ++i) {
                result.setElement(0, i, Value(range.row1 + i));
            }
            return result;
        }
    }
    if (e && args.count()) {
        row = e->ranges[0].row1;
    }
    if (row > 0)
        return Value(row);
    return Value::errorVALUE();
}

//
// Function: ROWS
//
Value func_rows(valVector, ValueCalc *, FuncExtra *e)
{
    int row1 = e->ranges[0].row1;
    int row2 = e->ranges[0].row2;
    if ((row1 == -1) || (row2 == -1))
        return Value::errorVALUE();
    return Value(row2 - row1 + 1);
}

//
// Function: SHEET
//
Value func_sheet(valVector args, ValueCalc *calc, FuncExtra *e)
{
    Q_UNUSED(calc);
    SheetBase *sheet = e->sheet;
    if (!args.isEmpty() && args[0].isString() && (e->regions.isEmpty() || !e->regions[0].isValid())) {
        sheet = e->sheet->map()->findSheet(args[0].asString());
        if (!sheet) {
            return Value::errorNA();
        }
    }
    if (!e->regions.isEmpty()) {
        const Calligra::Sheets::Region &region = e->regions[0];
        if (region.isValid())
            sheet = region.firstSheet();
    }
    return Value(sheet->map()->indexOf(sheet) + 1);
}

//
// Function: SHEETS
//
Value func_sheets(valVector args, ValueCalc *, FuncExtra *e)
{
    if (args.isEmpty() && !e->ranges.isEmpty()) {
        return Value::errorVALUE();
    }
    if (!args.isEmpty() && (args[0].isArray() || args[0].isString()) && (e->regions.isEmpty() || !e->regions[0].isValid())) {
        return Value::errorVALUE();
    }
    if (!args.isEmpty() && args[0].isEmpty()) {
        return Value::errorVALUE();
    }
    if (!e->regions.isEmpty()) {
        const Calligra::Sheets::Region &region = e->regions[0];
        if (region.isValid()) {
            if (region.rects().size() > 1) {
                return Value::errorVALUE();
            }
            if (region.firstSheet() != region.lastSheet()) {
                return Value::errorVALUE();
            }
            QList<Calligra::Sheets::SheetBase *> sheets;
            Calligra::Sheets::Region::ConstIterator it(region.constBegin()), end(region.constEnd());
            for (; it != end; ++it)
                if (!sheets.contains((*it)->sheet()))
                    sheets.append((*it)->sheet());
            if (sheets.size() > 1) {
                return Value::errorVALUE();
            }
            return Value(sheets.count());
        }
    }
    int count = 0;
    for (SheetBase *sheet : e->sheet->map()->sheetList()) {
        // External document tabs are placeholders, not sheets in this workbook.
        if (!sheet->sheetName().contains(u'#')) {
            ++count;
        }
    }
    return Value(count);
}

//
// Function: VLOOKUP
//
Value func_vlookup(valVector args, ValueCalc *calc, FuncExtra *e)
{
    const Value key = args[0];
    Value data = args[1];
    if (e->regions.size() > 1 && e->regions[1].isValid()) {
        const auto &region = e->regions[1];
        if (region.rects().size() == 1) {
            const QRect rect = region.firstRange();
            data = Value(Value::Array);
            for (int row = 0; row < rect.height(); ++row) {
                for (int col = 0; col < rect.width(); ++col) {
                    data.setElement(col, row, CellBase(region.firstSheet(), rect.left() + col, rect.top() + row).value());
                }
            }
        }
    }
    if (key.isArray()) {
        Value result(Value::Array);
        for (unsigned row = 0; row < key.rows(); ++row) {
            for (unsigned col = 0; col < key.columns(); ++col) {
                valVector scalarArgs = args;
                scalarArgs[0] = key.element(col, row);
                result.setElement(col, row, func_vlookup(scalarArgs, calc, e));
            }
        }
        return result;
    }
    if (e->ranges.size() > 1) {
        auto range = e->ranges[1];
        if (range.rows() > 1 || range.columns() > 1) {
            data = Value(Value::Array);
            for (int row = 0; row < range.rows(); ++row) {
                for (int col = 0; col < range.columns(); ++col) {
                    data.setElement(col, row, CellBase(e->sheet, range.col1 + col, range.row1 + row).value());
                }
            }
        }
    }
    const int col = calc->conv()->asInteger(args[2]).asInteger();
    const int cols = data.columns();
    const int rows = data.rows();
    if (col < 1 || col > cols)
        return Value::errorVALUE();
    const bool rangeLookup = (args.count() > 3) ? calc->conv()->asBoolean(args[3]).asBoolean() : true;
    if (key.isEmpty() && rangeLookup) {
        return Value::errorNA();
    }

    // now traverse the array and perform comparison
    Value r;
    Value v = Value::errorNA();
    const auto folded = [](QString value) {
        value.replace(u"\\."_s, u"."_s);
        value.replace(u"\\n"_s, u""_s);
        value = value.normalized(QString::NormalizationForm_D);
        QString result;
        for (const QChar ch : value) {
            if (ch.category() != QChar::Mark_NonSpacing) {
                result += ch.toLower();
            }
        }
        return result;
    };
    for (int row = 0; row < rows; ++row) {
        // search in the first column
        const Value le = data.element(0, row);
        const bool asciiStringEqual = key.isString() && le.isString() && folded(key.asString()) == folded(le.asString())
            && std::all_of(key.asString().cbegin(),
                           key.asString().cend(),
                           [](QChar ch) {
                               return ch.unicode() < 128;
                           })
            && std::all_of(le.asString().cbegin(), le.asString().cend(), [](QChar ch) {
                                          return ch.unicode() < 128;
                                      });
        const bool escapedStringEqual = key.isString() && le.isString() && folded(key.asString()) == folded(le.asString())
            && (key.asString().contains(u'\\') || le.asString().contains(u'\\'));
        const bool turkishCase = key.isString() && le.isString()
            && (key.asString().contains(QChar(0x130)) || key.asString().contains(QChar(0x131)) || le.asString().contains(QChar(0x130))
                || le.asString().contains(QChar(0x131)));
        if (asciiStringEqual || escapedStringEqual || calc->naturalEqual(key, le, !turkishCase)) {
            return data.element(col - 1, row);
        }
        // optionally look for the next largest value that is less than key
        const bool comparable = (key.isNumber() && le.isNumber()) || (key.isString() && le.isString()) || (key.isBoolean() && le.isBoolean());
        const bool lower = key.isString() && le.isString() ? folded(le.asString()) < folded(key.asString()) : calc->naturalLower(le, key);
        const bool previousLower = r.isEmpty() || calc->naturalLower(r, le);
        if (rangeLookup && comparable && lower && previousLower) {
            r = le;
            v = data.element(col - 1, row);
        }
    }
    return v;
}

#include "reference.moc"
