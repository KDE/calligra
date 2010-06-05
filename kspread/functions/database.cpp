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

// built-in database functions

#include "DatabaseModule.h"
#include "FunctionModuleRegistry.h"
#include "Functions.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <KGenericFactory>
#include <KLocale>

using namespace KSpread;

// prototypes
Value func_daverage(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dcount(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dcounta(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dget(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dmax(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dmin(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dproduct(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dstdev(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dstdevp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dsum(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dvar(valVector args, ValueCalc *calc, FuncExtra *);
Value func_dvarp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_getpivotdata(valVector args, ValueCalc *calc, FuncExtra *);


KSPREAD_EXPORT_FUNCTION_MODULE("database", DatabaseModule)


DatabaseModule::DatabaseModule(QObject* parent, const QVariantList&)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("DAVERAGE",     func_daverage);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DCOUNT",       func_dcount);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DCOUNTA",      func_dcounta);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DGET",         func_dget);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DMAX",         func_dmax);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DMIN",         func_dmin);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DPRODUCT",     func_dproduct);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DSTDEV",       func_dstdev);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DSTDEVP",      func_dstdevp);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DSUM",         func_dsum);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DVAR",         func_dvar);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("DVARP",        func_dvarp);
    f->setParamCount(3);
    f->setAcceptArray();
  add(f);
    f = new Function("GETPIVOTDATA", func_getpivotdata);  // partially Excel-compatible
    f->setParamCount(2);
    f->setAcceptArray();
  add(f);
}

QString DatabaseModule::descriptionFileName() const
{
    return QString("database.xml");
}


int getFieldIndex(ValueCalc *calc, Value fieldName,
                  Value database)
{
    if (fieldName.isNumber())
        return fieldName.asInteger() - 1;
    if (!fieldName.isString())
        return -1;

    QString fn = fieldName.asString();
    int cols = database.columns();
    for (int i = 0; i < cols; ++i)
        if (fn.toLower() ==
                calc->conv()->asString(database.element(i, 0)).asString().toLower())
            return i;
    return -1;
}

// ***********************************************************
// *** DBConditions class - maintains an array of conditions ***
// ***********************************************************

class DBConditions
{
public:
    DBConditions(ValueCalc *vc, Value database, Value conds);
    ~DBConditions();
    /** Does a specified row of the database match the given criteria?
    The row with column names is ignored - hence 0 specifies first data row. */
    bool matches(unsigned row);
private:
    void parse(Value conds);
    ValueCalc *calc;
    Condition **cond;
    int rows, cols;
    Value db;
};

DBConditions::DBConditions(ValueCalc *vc, Value database,
                           Value conds) : calc(vc), cond(0), rows(0), cols(0), db(database)
{
    parse(conds);
}

DBConditions::~DBConditions()
{
    int count = rows * cols;
    for (int r = 0; r < count; ++r)
        delete cond[r];
    delete[] cond;
}

void DBConditions::parse(Value conds)
{
    // initialize the array
    rows = conds.rows() - 1;
    cols = db.columns();
    int count = rows * cols;

    // if rows or cols is zero or negative, then we don't need to parse anything
    if(count <= 0)
        return;
    
    cond = new Condition* [count];
    for (int r = 0; r < count; ++r)
        cond[r] = 0;

    // perform the parsing itself
    int cc = conds.columns();
    for (int c = 0; c < cc; ++c) {
        // first row contains column names
        int col = getFieldIndex(calc, conds.element(c, 0), db);
        if (col < 0) continue;  // failed - ignore the column

        // fill in the conditions for a given column name
        for (int r = 0; r < rows; ++r) {
            Value cnd = conds.element(c, r + 1);
            if (cnd.isEmpty()) continue;
            int idx = r * cols + col;
            if (cond[idx]) delete cond[idx];
            cond[idx] = new Condition;
            calc->getCond(*cond[idx], cnd);
        }
    }
}

bool DBConditions::matches(unsigned row)
{
    if (row >= db.rows() - 1)
        return false;    // out of range

    // we have a match, if at least one row of criteria matches
    for (int r = 0; r < rows; ++r) {
        // within a row, all criteria must match
        bool match = true;
        for (int c = 0; c < cols; ++c) {
            int idx = r * cols + c;
            if (!cond[idx]) continue;
            if (!calc->matches(*cond[idx], db.element(c, row + 1))) {
                match = false;  // didn't match
                break;
            }
        }
        if (match)  // all conditions in this row matched
            return true;
    }

    // no row matched
    return false;
}


// *******************************************
// *** Function implementations start here ***
// *******************************************

// Function: DSUM
Value func_dsum(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value res(0.0);
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty())
                res = calc->add(res, val);
        }

    return res;
}

// Function: DAVERAGE
Value func_daverage(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value res;
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                res = calc->add(res, val);
                count++;
            }
        }
    if (count) res = calc->div(res, count);
    return res;
}

// Function: DCOUNT
Value func_dcount(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
                count++;
        }

    return Value(count);
}

// Function: DCOUNTA
Value func_dcounta(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty())
                count++;
        }

    return Value(count);
}

// Function: DGET
Value func_dget(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    bool match = false;
    Value result = Value::errorVALUE();
    int rows = database.rows() - 1;  // first row contains column names
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            if (match) {
                // error on multiple matches
                result = Value::errorVALUE();
                break;
            }
            result = database.element(fieldIndex, r + 1);
            match = true;
        }

    return result;
}

// Function: DMAX
Value func_dmax(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value res;
    bool got = false;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                if (!got) {
                    res = val;
                    got = true;
                } else
                    if (calc->greater(val, res))
                        res = val;
            }
        }

    return res;
}

// Function: DMIN
Value func_dmin(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value res;
    bool got = false;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                if (!got) {
                    res = val;
                    got = true;
                } else
                    if (calc->lower(val, res))
                        res = val;
            }
        }

    return res;
}

// Function: DPRODUCT
Value func_dproduct(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value res = Value((double)1.0);
    bool got = false;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                got = true;
                res = calc->mul(res, val);
            }
        }
    if (got)
        return res;
    return Value::errorVALUE();
}

// Function: DSTDEV
Value func_dstdev(valVector args, ValueCalc *calc, FuncExtra *)
{
    // sqrt (dvar)
    return calc->sqrt(func_dvar(args, calc, 0));
}

// Function: DSTDEVP
Value func_dstdevp(valVector args, ValueCalc *calc, FuncExtra *)
{
    // sqrt (dvarp)
    return calc->sqrt(func_dvarp(args, calc, 0));
}

// Function: DVAR
Value func_dvar(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value avg;
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                avg = calc->add(avg, val);
                count++;
            }
        }
    if (count < 2) return Value::errorDIV0();
    avg = calc->div(avg, count);

    Value res;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty())
                res = calc->add(res, calc->sqr(calc->sub(val, avg)));
        }

    // res / (count-1)
    return calc->div(res, count - 1);
}

// Function: DVARP
Value func_dvarp(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1;  // first row contains column names
    Value avg;
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty()) {
                avg = calc->add(avg, val);
                count++;
            }
        }
    if (count == 0) return Value::errorDIV0();
    avg = calc->div(avg, count);

    Value res;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            Value val = database.element(fieldIndex, r + 1);
            // include this value in the result
            if (!val.isEmpty())
                res = calc->add(res, calc->sqr(calc->sub(val, avg)));
        }

    // res / count
    return calc->div(res, count);
}

// Function: GETPIVOTDATA
// FIXME implement more things with this, see Excel !
Value func_getpivotdata(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    int fieldIndex = getFieldIndex(calc, args[1], database);
    if (fieldIndex < 0)
        return Value::errorVALUE();
    // the row at the bottom
    int row = database.rows() - 1;

    return database.element(fieldIndex, row);
}

#include "DatabaseModule.moc"
