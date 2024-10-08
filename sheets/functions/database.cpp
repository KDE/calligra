// This file is part of the KDE project
// SPDX-FileCopyrightText: 1998-2002 The KSpread Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// built-in database functions

#include "DatabaseModule.h"

#include "engine/Function.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

using namespace Calligra::Sheets;

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

CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE("kspreaddatabasemodule.json", DatabaseModule)

DatabaseModule::DatabaseModule(QObject *parent, const QVariantList &)
    : FunctionModule(parent)
{
    Function *f;

    f = new Function("DAVERAGE", func_daverage);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DCOUNT", func_dcount);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DCOUNTA", func_dcounta);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DGET", func_dget);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DMAX", func_dmax);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DMIN", func_dmin);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DPRODUCT", func_dproduct);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DSTDEV", func_dstdev);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DSTDEVP", func_dstdevp);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DSUM", func_dsum);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DVAR", func_dvar);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("DVARP", func_dvarp);
    f->setParamCount(3);
    f->setAcceptArray();
    add(f);
    f = new Function("GETPIVOTDATA", func_getpivotdata); // partially Excel-compatible
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
}

QString DatabaseModule::descriptionFileName() const
{
    return QString("database.xml");
}

int getFieldIndex(ValueCalc *calc, Value fieldName, Value database)
{
    if (fieldName.isNumber())
        return fieldName.asInteger() - 1;
    if (!fieldName.isString())
        return -1;

    QString fn = fieldName.asString();
    int cols = database.columns();
    for (int i = 0; i < cols; ++i)
        if (fn.toLower() == calc->conv()->asString(database.element(i, 0)).asString().toLower())
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
    QList<QList<Condition *>> cond;
    int rows, cols;
    Value db;
};

DBConditions::DBConditions(ValueCalc *vc, Value database, Value conds)
    : calc(vc)
    , rows(0)
    , cols(0)
    , db(database)
{
    parse(conds);
}

DBConditions::~DBConditions()
{
    int count = rows * cols;
    for (int r = 0; r < count; ++r)
        qDeleteAll(cond[r]);
}

void DBConditions::parse(Value conds)
{
    // initialize the array
    rows = conds.rows() - 1;
    cols = db.columns();
    int count = rows * cols;

    // if rows or cols is zero or negative, then we don't need to parse anything
    if (count <= 0)
        return;

    for (int r = 0; r < count; ++r)
        cond.append(QList<Condition *>());

    // perform the parsing itself
    int cc = conds.columns();
    for (int c = 0; c < cc; ++c) {
        // first row contains column names
        int col = getFieldIndex(calc, conds.element(c, 0), db);
        if (col < 0)
            continue; // failed - ignore the column

        // fill in the conditions for a given column name
        for (int r = 0; r < rows; ++r) {
            Value cnd = conds.element(c, r + 1);
            if (cnd.isEmpty())
                continue;
            int idx = r * cols + col;
            // if (cond[idx]) delete cond[idx];
            Condition *theCond = new Condition;
            calc->getCond(*theCond, cnd);
            cond[idx].append(theCond);
        }
    }
}

bool DBConditions::matches(unsigned row)
{
    if (row >= db.rows() - 1)
        return false; // out of range

    // we have a match, if at least one row of criteria matches
    for (int r = 0; r < rows; ++r) {
        // within a row, all criteria must match
        bool match = true;
        for (int c = 0; c < cols; ++c) {
            int idx = r * cols + c;
            if (cond[idx].isEmpty())
                continue;
            for (int i = 0; i < cond[idx].size(); i++) {
                if (!calc->matches(*cond[idx][i], db.element(c, row + 1))) {
                    match = false; // didn't match
                    break;
                }
            }
        }
        if (match) // all conditions in this row matched
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

    int rows = database.rows() - 1; // first row contains column names
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

    int rows = database.rows() - 1; // first row contains column names
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
    if (count)
        res = calc->div(res, count);
    return res;
}

// Function: DCOUNT
Value func_dcount(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1; // first row contains column names
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            // fieldIndex is optional, if no field is specified count all rows matching the conditions
            if (fieldIndex < 0)
                count++;
            else {
                Value val = database.element(fieldIndex, r + 1);
                // include this value in the result
                if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
                    count++;
            }
        }

    return Value(count);
}

// Function: DCOUNTA
Value func_dcounta(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value database = args[0];
    Value conditions = args[2];
    int fieldIndex = getFieldIndex(calc, args[1], database);

    DBConditions conds(calc, database, conditions);

    int rows = database.rows() - 1; // first row contains column names
    int count = 0;
    for (int r = 0; r < rows; ++r)
        if (conds.matches(r)) {
            // fieldIndex is optional, if no field is specified count all rows matching the conditions
            if (fieldIndex < 0)
                count++;
            else {
                Value val = database.element(fieldIndex, r + 1);
                // include this value in the result
                if (!val.isEmpty())
                    count++;
            }
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
    int rows = database.rows() - 1; // first row contains column names
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

    int rows = database.rows() - 1; // first row contains column names
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
                } else if (calc->greater(val, res))
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

    int rows = database.rows() - 1; // first row contains column names
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
                } else if (calc->lower(val, res))
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

    int rows = database.rows() - 1; // first row contains column names
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
    return calc->sqrt(func_dvar(args, calc, nullptr));
}

// Function: DSTDEVP
Value func_dstdevp(valVector args, ValueCalc *calc, FuncExtra *)
{
    // sqrt (dvarp)
    return calc->sqrt(func_dvarp(args, calc, nullptr));
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

    int rows = database.rows() - 1; // first row contains column names
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
    if (count < 2)
        return Value::errorDIV0();
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

    int rows = database.rows() - 1; // first row contains column names
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
    if (count == 0)
        return Value::errorDIV0();
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

#include "database.moc"
