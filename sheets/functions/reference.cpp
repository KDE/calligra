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

using namespace Calligra::Sheets;

// prototypes (sorted alphabetically)
Value func_address(valVector args, ValueCalc *calc, FuncExtra *);
Value func_areas(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cell(valVector args, ValueCalc *calc, FuncExtra *e);
Value func_choose(valVector args, ValueCalc *calc, FuncExtra *);
Value func_column(valVector args, ValueCalc *calc, FuncExtra *);
Value func_columns(valVector args, ValueCalc *calc, FuncExtra *);
Value func_hlookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_index(valVector args, ValueCalc *calc, FuncExtra *);
Value func_indirect(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lookup(valVector args, ValueCalc *calc, FuncExtra *);
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
    add(f);
    f = new Function("COLUMNS", func_columns);
    f->setParamCount(1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("HLOOKUP", func_hlookup);
    f->setParamCount(3, 4);
    f->setAcceptArray();
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
    if (args.count() > 2)
        absNum = calc->conv()->asInteger(args[2]).asInteger();
    if (args.count() > 3)
        r1c1 = !(calc->conv()->asBoolean(args[3]).asBoolean());
    if (args.count() == 5)
        sheetName = calc->conv()->asString(args[4]).asString();

    QString result;
    int row = calc->conv()->asInteger(args[0]).asInteger();
    int col = calc->conv()->asInteger(args[1]).asInteger();

    if (!sheetName.isEmpty()) {
        result += sheetName;
        result += '!';
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
        if (rangeLookup && calc->naturalLower(le, key) && calc->naturalLower(r, le)) {
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
    unsigned row = calc->conv()->asInteger(args[1]).asInteger() - 1;
    unsigned col = calc->conv()->asInteger(args[2]).asInteger() - 1;
    if ((row >= val.rows()) || (col >= val.columns()))
        return Value::errorREF();
    return val.element(col, row);
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
        // TODO: translate the r1c1 style to a1 style
        ref = ref;
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
Value func_lookup(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value num = calc->conv()->asNumeric(args[0]);
    if (num.isArray())
        return Value::errorVALUE();
    Value lookup = args[1];
    Value rr = args[2];
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
            if (calc->lower(le, num) || calc->equal(num, le))
                res = rr.element(c, r);
            else
                return res;
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
    const Value &searchArray = args[1];

    if (e->ranges[1].rows() != 1 && e->ranges[1].columns() != 1)
        return Value::errorNA();
    int dr = 1, dc = 0;
    if (searchArray.columns() != 1) {
        dr = 0;
        dc = 1;
    }
    int n = qMax(searchArray.rows(), searchArray.columns());

    if (matchType == 0) {
        // linear search
        for (int r = 0, c = 0; r < n && c < n; r += dr, c += dc) {
            if (calc->naturalEqual(searchValue, searchArray.element(c, r), false)) {
                return Value(qMax(r, c) + 1);
            }
        }
        return Value::errorNA();
    } else if (matchType > 0) {
        // binary search
        int l = -1;
        int h = n;
        while (l + 1 < h) {
            int m = (l + h) / 2;
            if (calc->naturalLequal(searchArray.element(m * dc, m * dr), searchValue, false)) {
                l = m;
            } else {
                h = m;
            }
        }
        if (l == -1)
            return Value::errorNA();
        return Value(l + 1);
    } else /* matchType < 0 */ {
        // binary search
        int l = -1;
        int h = n;
        while (l + 1 < h) {
            int m = (l + h) / 2;
            if (calc->naturalGequal(searchArray.element(m * dc, m * dr), searchValue, false)) {
                l = m;
            } else {
                h = m;
            }
        }
        if (l == -1)
            return Value::errorNA();
        return Value(l + 1);
    }
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

    // const int rowNew = args.count() >= 4 ? calc->conv()->asInteger(args[3]).asInteger() : -1;
    // const int colNew = args.count() >= 5 ? calc->conv()->asInteger(args[4]).asInteger() : -1;
    // if (colNew == 0 || rowNew == 0) return Value::errorVALUE();

    // Doesn't take references to other sheets into account
    // const QRect rect(e->ranges[0].col1, e->ranges[0].row1, e->ranges[0].col2, e->ranges[0].row2);
    // const Calligra::Sheets::Region region(rect, e->sheet);

    if (e->regions.isEmpty())
        return Value::errorVALUE();

    const Calligra::Sheets::Region &region = e->regions[0];

    if (!region.isValid() /* || !region.isSingular() */)
        return Value::errorVALUE();

    QPoint p = region.firstRange().topLeft() + QPoint(colPlus, rowPlus);
    const CellBase cell(region.firstSheet(), p);
    if (!cell.isNull())
        return cell.value();

    return Value::errorVALUE();
}

//
// Function: ROW
//
Value func_row(valVector args, ValueCalc *, FuncExtra *e)
{
    int row = e ? e->myrow : 0;
    if (e && args.count())
        row = e->ranges[0].row1;
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
Value func_sheet(valVector /*args*/, ValueCalc *, FuncExtra *e)
{
    SheetBase *sheet = e->sheet;
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
Value func_sheets(valVector /*args*/, ValueCalc *, FuncExtra *e)
{
    if (!e->regions.isEmpty()) {
        const Calligra::Sheets::Region &region = e->regions[0];
        if (region.isValid()) {
            QList<Calligra::Sheets::SheetBase *> sheets;
            Calligra::Sheets::Region::ConstIterator it(region.constBegin()), end(region.constEnd());
            for (; it != end; ++it)
                if (!sheets.contains((*it)->sheet()))
                    sheets.append((*it)->sheet());
            return Value(sheets.count());
        }
    }
    return Value(e->sheet->map()->count());
}

//
// Function: VLOOKUP
//
Value func_vlookup(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value key = args[0];
    const Value data = args[1];
    const int col = calc->conv()->asInteger(args[2]).asInteger();
    const int cols = data.columns();
    const int rows = data.rows();
    if (col < 1 || col > cols)
        return Value::errorVALUE();
    const bool rangeLookup = (args.count() > 3) ? calc->conv()->asBoolean(args[3]).asBoolean() : true;

    // now traverse the array and perform comparison
    Value r;
    Value v = Value::errorNA();
    for (int row = 0; row < rows; ++row) {
        // search in the first column
        const Value le = data.element(0, row);
        if (calc->naturalEqual(key, le)) {
            return data.element(col - 1, row);
        }
        // optionally look for the next largest value that is less than key
        if (rangeLookup && calc->naturalLower(le, key) && calc->naturalLower(r, le)) {
            r = le;
            v = data.element(col - 1, row);
        }
    }
    return v;
}

#include "reference.moc"
