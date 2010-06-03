/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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

// built-in reference functions

#include "Cell.h"
#include "Region.h"
#include "Sheet.h"
#include "Util.h"
#include "Value.h"

#include "CellStorage.h"
#include "Formula.h"
#include "FunctionModuleRegistry.h"
#include "Functions.h"
#include "ReferenceModule.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_address(valVector args, ValueCalc *calc, FuncExtra *);
Value func_areas(valVector args, ValueCalc *calc, FuncExtra *);
Value func_choose(valVector args, ValueCalc *calc, FuncExtra *);
Value func_column(valVector args, ValueCalc *calc, FuncExtra *);
Value func_columns(valVector args, ValueCalc *calc, FuncExtra *);
Value func_hlookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_index(valVector args, ValueCalc *calc, FuncExtra *);
Value func_indirect(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lookup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_multiple_operations(valVector args, ValueCalc *calc, FuncExtra *);
Value func_row(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rows(valVector args, ValueCalc *calc, FuncExtra *);
Value func_vlookup(valVector args, ValueCalc *calc, FuncExtra *);


#ifndef KSPREAD_UNIT_TEST // Do not create/export the plugin in unit tests.
K_PLUGIN_FACTORY(ReferenceModulePluginFactory,
                 registerPlugin<ReferenceModule>();
                )
K_EXPORT_PLUGIN(ReferenceModulePluginFactory("ReferenceModule"))
#endif


ReferenceModule::ReferenceModule(QObject* parent, const QVariantList&)
        : FunctionModule(parent, "reference", i18n("Reference Functions"))
{
}

QString ReferenceModule::descriptionFileName() const
{
    return QString("reference.xml");
}

void ReferenceModule::registerFunctions()
{
    FunctionRepository* repo = FunctionRepository::self();
    Function *f;

    f = new Function("ADDRESS",  func_address);
    f->setParamCount(2, 5);
    repo->add(f);
    f = new Function("AREAS",    func_areas);
    f->setParamCount(1);
    f->setNeedsExtra(true);
    f->setAcceptArray();
    repo->add(f);
    f = new Function("CHOOSE",   func_choose);
    f->setParamCount(2, -1);
    f->setAcceptArray();
    repo->add(f);
    f = new Function("COLUMN",   func_column);
    f->setParamCount(0, 1);
    repo->add(f);
    f = new Function("COLUMNS",  func_columns);
    f->setParamCount(1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    repo->add(f);
    f = new Function("HLOOKUP",  func_hlookup);
    f->setParamCount(3, 4);
    f->setAcceptArray();
    repo->add(f);
    f = new Function("INDEX",   func_index);
    f->setParamCount(3);
    f->setAcceptArray();
    repo->add(f);
    f = new Function("INDIRECT", func_indirect);
    f->setParamCount(1, 2);
    f->setNeedsExtra(true);
    repo->add(f);
    f = new Function("LOOKUP",   func_lookup);
    f->setParamCount(3);
    f->setAcceptArray();
    repo->add(f);
    f = new Function("MULTIPLE.OPERATIONS", func_multiple_operations);
    f->setParamCount(3, 5);
    f->setNeedsExtra(true);
    repo->add(f);
    f = new Function("ROW",      func_row);
    f->setParamCount(0, 1);
    repo->add(f);
    f = new Function("ROWS",     func_rows);
    f->setParamCount(1);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    repo->add(f);
    f = new Function("VLOOKUP",  func_vlookup);
    f->setParamCount(3, 4);
    f->setAcceptArray();
    repo->add(f);
}

void ReferenceModule::removeFunctions()
{
    // NOTE: The group name has to match the one in the xml description.
    FunctionRepository::self()->remove("Lookup & Reference");
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

        result += Cell::columnName(col);

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

        if ((e->ranges[0].col1 != -1) && (e->ranges[0].row1 != -1) &&
                (e->ranges[0].col2 != -1) && (e->ranges[0].row2 != -1))
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
            if (!KSpread::Region(ref).isValid())
                return Value::errorVALUE();
            else {
                ++num;
                ref = "";
            }
        } else
            ref += s[i];
    }

    return Value(num);
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
    const bool sorted = (args.count() > 3) ? calc->conv()->asBoolean(args[3]).asBoolean() : true;

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
        if (sorted && calc->naturalLower(le, key) && calc->naturalLower(r, le)) {
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

    const KSpread::Region region(ref, e->sheet->map(), e->sheet);
    if (!region.isValid() || !region.isSingular())
        return Value::errorVALUE();

    const Cell cell(region.firstSheet(), region.firstRange().topLeft());
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

    CellStorage *s = e->sheet->cellStorage();

    // get formula to evaluate
    int formulaCol = e->ranges[0].col1;
    int formulaRow = e->ranges[0].row1;
    Formula formula = s->formula(formulaCol, formulaRow);
    if (!formula.isValid())
        return Value::errorVALUE();

    CellIndirection cellIndirections;
    cellIndirections.insert(Cell(e->sheet, e->ranges[1].col1, e->ranges[1].row1), Cell(e->sheet, e->ranges[2].col1, e->ranges[2].row1));
    if (args.count() > 3) {
        cellIndirections.insert(Cell(e->sheet, e->ranges[3].col1, e->ranges[3].row1), Cell(e->sheet, e->ranges[4].col1, e->ranges[4].row1));
    }

    return formula.eval(cellIndirections);
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
    const bool sorted = (args.count() > 3) ? calc->conv()->asBoolean(args[3]).asBoolean() : true;

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
        if (sorted && calc->naturalLower(le, key) && calc->naturalLower(r, le)) {
            r = le;
            v = data.element(col - 1, row);
        }
    }
    return v;
}

#include "ReferenceModule.moc"
