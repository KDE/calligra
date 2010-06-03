/* This file is part of the KDE project
   Copyright 2007 Brad Hards <bradh@frogmouth.net>
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>

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

#include <CellStorage.h>
#include <part/Doc.h> // FIXME detach from part
#include <Formula.h>
#include <Map.h>
#include <Sheet.h>

#include "TestKspreadCommon.h"

#include "TestInformationFunctions.h"

#include "functions/DateTimeModule.h"
#include "functions/InformationModule.h"
#include "functions/LogicModule.h"
#include "functions/MathModule.h"
#include "functions/ReferenceModule.h"
#include "functions/TextModule.h"
#include "FunctionModuleRegistry.h"

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestInformationFunctions::evaluate(const QString& formula, Value& ex)
{
    Sheet* sheet = m_doc->map()->sheet(0);
    Formula f(sheet);
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

    if (result.isFloat() && ex.isInteger())
        ex = Value(ex.asFloat());
    if (result.isInteger() && ex.isFloat())
        result = Value(result.asFloat());

    return result;
}

void TestInformationFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->add(new DateTimeModule(this));
    FunctionModuleRegistry::instance()->add(new InformationModule(this));
    FunctionModuleRegistry::instance()->add(new LogicModule(this));
    FunctionModuleRegistry::instance()->add(new MathModule(this));
    FunctionModuleRegistry::instance()->add(new ReferenceModule(this));
    FunctionModuleRegistry::instance()->add(new TextModule(this));
    FunctionModuleRegistry::instance()->registerFunctions();
    m_doc = new Doc();
    m_doc->map()->addNewSheet();
    Sheet* sheet = m_doc->map()->sheet(0);
    sheet->setSheetName("Sheet1");
    CellStorage* storage = sheet->cellStorage();

    //
    // Test case data set
    //


//     // A19:A29
//     storage->setValue(1,19, Value(    1 ) );
//     storage->setValue(1,20, Value(    2 ) );
//     storage->setValue(1,21, Value(    4 ) );
//     storage->setValue(1,22, Value(    8 ) );
//     storage->setValue(1,23, Value(   16 ) );
//     storage->setValue(1,24, Value(   32 ) );
//     storage->setValue(1,25, Value(   64 ) );
//     storage->setValue(1,26, Value(  128 ) );
//     storage->setValue(1,27, Value(  256 ) );
//     storage->setValue(1,28, Value(  512 ) );
//     storage->setValue(1,29, Value( 1024 ) );
//     storage->setValue(1,30, Value( 2048 ) );
//     storage->setValue(1,31, Value( 4096 ) );
//
//
//     // B3:B17
    storage->setValue(2, 3, Value("7"));
    storage->setValue(2, 4, Value(2));
    storage->setValue(2, 5, Value(3));
    storage->setValue(2, 6, Value(true));
    storage->setValue(2, 7, Value("Hello"));
//     // B8 leave empty
    storage->setValue(2, 9, Value::errorDIV0());
    storage->setValue(2, 10, Value(0));
//     storage->setValue(2,11, Value(      3    ) );
//     storage->setValue(2,12, Value(      4    ) );
//     storage->setValue(2,13, Value( "2005-0131T01:00:00" ));
//     storage->setValue(2,14, Value(      1    ) );
//     storage->setValue(2,15, Value(      2    ) );
//     storage->setValue(2,16, Value(      3    ) );
//     storage->setValue(2,17, Value(      4    ) );
//
//
//     // C4:C7
    storage->setValue(3, 4, Value(4));
    storage->setValue(3, 5, Value(5));
//     storage->setValue(3, 6, Value( 7 ) );
    storage->setValue(3, 7, Value("2005-01-31"));
//     // C11:C17
//     storage->setValue(3,11, Value( 5 ) );
//     storage->setValue(3,12, Value( 6 ) );
//     storage->setValue(3,13, Value( 8 ) );
//     storage->setValue(3,14, Value( 4 ) );
//     storage->setValue(3,15, Value( 3 ) );
//     storage->setValue(3,16, Value( 2 ) );
//     storage->setValue(3,17, Value( 1 ) );
//     // C19:C31
//     storage->setValue(3,19, Value( 0 ) );
//     storage->setValue(3,20, Value( 5 ) );
//     storage->setValue(3,21, Value( 2 ) );
//     storage->setValue(3,22, Value( 5 ) );
//     storage->setValue(3,23, Value( 3 ) );
//     storage->setValue(3,24, Value( 4 ) );
//     storage->setValue(3,25, Value( 4 ) );
//     storage->setValue(3,26, Value( 0 ) );
//     storage->setValue(3,27, Value( 8 ) );
//     storage->setValue(3,28, Value( 1 ) );
//     storage->setValue(3,29, Value( 9 ) );
//     storage->setValue(3,30, Value( 6 ) );
//     storage->setValue(3,31, Value( 2 ) );
//     // C51:C57
//     storage->setValue(3,51, Value(  7 ) );
//     storage->setValue(3,52, Value(  9 ) );
//     storage->setValue(3,53, Value( 11 ) );
//     storage->setValue(3,54, Value( 12 ) );
//     storage->setValue(3,55, Value( 15 ) );
//     storage->setValue(3,56, Value( 17 ) );
//     storage->setValue(3,57, Value( 19 ) );
//
//     // D51:D57
//     storage->setValue(4,51, Value( 100 ) );
//     storage->setValue(4,52, Value( 105 ) );
//     storage->setValue(4,53, Value( 104 ) );
//     storage->setValue(4,54, Value( 108 ) );
//     storage->setValue(4,55, Value( 111 ) );
//     storage->setValue(4,56, Value( 120 ) );
//     storage->setValue(4,57, Value( 133 ) );
//
//
//     // F51:F60
//     storage->setValue(6,51, Value( 3 ) );
//     storage->setValue(6,52, Value( 4 ) );
//     storage->setValue(6,53, Value( 5 ) );
//     storage->setValue(6,54, Value( 2 ) );
//     storage->setValue(6,55, Value( 3 ) );
//     storage->setValue(6,56, Value( 4 ) );
//     storage->setValue(6,57, Value( 5 ) );
//     storage->setValue(6,58, Value( 6 ) );
//     storage->setValue(6,59, Value( 4 ) );
//     storage->setValue(6,60, Value( 7 ) );
//
//
//     // G51:G60
//     storage->setValue(7,51, Value( 23 ) );
//     storage->setValue(7,52, Value( 24 ) );
//     storage->setValue(7,53, Value( 25 ) );
//     storage->setValue(7,54, Value( 22 ) );
//     storage->setValue(7,55, Value( 23 ) );
//     storage->setValue(7,56, Value( 24 ) );
//     storage->setValue(7,57, Value( 25 ) );
//     storage->setValue(7,58, Value( 26 ) );
//     storage->setValue(7,59, Value( 24 ) );
//     storage->setValue(7,60, Value( 27 ) );
}

//
// unittests
//

void TestInformationFunctions::testAREAS()
{
    CHECK_EVAL("AREAS(B3)",          Value(1));     // A reference to a single cell has one area
    CHECK_EVAL("AREAS(B3:C4)",       Value(1));     // A reference to a single range has one area
// concatenation is not supported yet
//    CHECK_EVAL( "AREAS(B3:C4~D5:D6)", Value( 2 ) ); // Cell concatenation creates multiple areas
//    CHECK_EVAL( "AREAS(B3:C4~B3)",    Value( 2 ) ); // Cell concatenation counts, even if the cells are duplicated
}

/*
void TestInformationFunctions::testCELL()
{
    CHECK_EVAL( "CELL(\"COL\";B7)",            Value( 2              ) ); // Column B is column number 2.
    CHECK_EVAL( "CELL(\"ADDRESS\";B7)",        Value( "$B$7"         ) ); // Absolute address
    CHECK_EVAL( "CELL(\"ADDRESS\";Sheet2!B7)", Value( "$Sheet2.$B$7" ) ); // Absolute address including sheet name

    // Absolute address including sheet name and IRI of location of documentare duplicated
    CHECK_EVAL( "CELL(\"ADDRESS\";'x:\\sample.ods'#Sheet3!B7)", Value( "'file:///x:/sample.ods'#$Sheet3.$B$7" ) );

    // The current cell is saved in a file named ��sample.ods�� which is located at ��file:///x:/��
    CHECK_EVAL( "CELL(\"FILENAME\")",          Value( "file:///x:/sample.ods" ) );

    CHECK_EVAL( "CELL(\"FORMAT\";C7)",         Value( "D4" ) ); // C7's number format is like ��DD-MM-YYYY HH:MM:SS��
}
*/

void TestInformationFunctions::testCOLUMN()
{
    CHECK_EVAL("COLUMN(B7)",       Value(2));     // Column "B" is column number 2.
//    CHECK_EVAL( "COLUMN()",         Value( 5 ) ); // Column of current cell is default, here formula in column E.

    Value res(Value::Array);
    res.setElement(0, 0, Value(2));
    res.setElement(1, 0, Value(3));
    res.setElement(2, 0, Value(4));
    CHECK_EVAL("COLUMN(B2:D2)", res);   // Array with column numbers.
}

void TestInformationFunctions::testCOLUMNS()
{
    CHECK_EVAL("COLUMNS(C1)",      Value(1));     // Single cell range contains one column.
    CHECK_EVAL("COLUMNS(C1:C4)",   Value(1));     // Range with only one column.
    CHECK_EVAL("COLUMNS(A4:D100)", Value(4));     // Number of columns in range.
}

void TestInformationFunctions::testCOUNT()
{
    CHECK_EVAL("COUNT(1;2;3)",       Value(3));     // Simple count.
    CHECK_EVAL("COUNT(B4:B5)",       Value(2));     // Two numbers in the range.
    CHECK_EVAL("COUNT(B4:B5;B4:B5)", Value(4));     // Duplicates are not removed.
    CHECK_EVAL("COUNT(B4:B9)",       Value(2));     // Errors in referenced cells or ranges are ignored.
    CHECK_EVAL("COUNT(B4:B8;1/0)",   Value(2));     // Errors in direct parameters are still ignored..
    CHECK_EVAL("COUNT(B3:B5)",       Value(2));     // Conversion to NumberSequence ignores strings (in B3).
}

void TestInformationFunctions::testCOUNTA()
{
    CHECK_EVAL("COUNTA(\"1\";2;TRUE())",     Value(3));     // Simple count of 3 constant values.
    CHECK_EVAL("COUNTA(B3:B5)",              Value(3));     // Three non-empty cells in the range.
    CHECK_EVAL("COUNTA(B3:B5;B3:B5)",        Value(6));     // Duplicates are not removed.
    CHECK_EVAL("COUNTA(B3:B9)",              Value(6));     // Where B9 is "=1/0", i.e. an error,
    // counts the error as non-empty; errors contained
    // in a reference do not propogate the error into the result.
    CHECK_EVAL("COUNTA(\"1\";2;SUM(B3:B9))", Value(3));     // Errors in an evaluated formula do not propagate; they are just counted.
    CHECK_EVAL("COUNTA(\"1\";2;B3:B9)",      Value(8));     // Errors in the range do not propagate either
}

void TestInformationFunctions::testCOUNTBLANK()
{
    CHECK_EVAL("COUNTBLANK(B3:B10)",    Value(1));     // Only B8 is blank. Zero ('0') in B10 is not considered blank.
}

void TestInformationFunctions::testCOUNTIF()
{
    CHECK_EVAL("COUNTIF(B4:B5;\">2.5\")", Value(1));           // B4 is 2 and B5 is 3, so there is one cell in the range with a value greater than 2.5.
    CHECK_EVAL("COUNTIF(B3:B5;B4)",       Value(1));           // Test if a cell equals the value in [.B4].
    CHECK_EVAL("COUNTIF(\"\";B4)",        Value::errorNA());   // Constant values are not allowed for the range.
    CHECK_EVAL("COUNTIF(B3:B10;\"7\")",   Value(1));           // [.B3] is the string "7".
    CHECK_EVAL("COUNTIF(B3:B10;1+1)",     Value(1));           // The criteria can be an expression.
}

void TestInformationFunctions::testERRORTYPE()
{
    CHECK_EVAL("ERRORTYPE(0)",    Value::errorVALUE());   // Non-errors produce an error.
    CHECK_EVAL("ERRORTYPE(NA())", Value(7));              // By convention, the ERROR.TYPE of NA() is 7.
    CHECK_EVAL("ERRORTYPE(1/0)",  Value(2));
}

/*
void TestInformationFunctions::testFORMULA()
{
    CHECK_EVAL( "LENGTH(FORMULA(B7))>0", Value( true ) ); // B7 is a formula, so this is fine and will produce a text value
    CHECK_EVAL( "FORMULA(B3)",           Value( true ) ); // Simple formulas that produce Text are still formulas
}
*/

void TestInformationFunctions::testINFO()
{
    CHECK_EVAL("INFO(\"recalc\")",             Value("Automatic"));     //
    CHECK_EVAL("ISTEXT(INFO(\"system\"))",     Value(true));            // The details of "system" vary by system, but it is always a text value
    CHECK_EVAL("ISTEXT(INFO(\"directory\"))",  Value(true));            // Test to see that every required category is supported
//     CHECK_EVAL( "ISNUMBER(INFO(\"memavail\"))", Value( true        ) ); // not implemented
//     CHECK_EVAL( "ISNUMBER(INFO(\"memused\"))",  Value( true        ) ); // not implemented
    CHECK_EVAL("ISNUMBER(INFO(\"numfile\"))",  Value(true));            //
    CHECK_EVAL("ISTEXT(INFO(\"osversion\"))",  Value(true));            //
//     CHECK_EVAL( "ISTEXT(INFO(\"origin\"))",     Value( true        ) ); // not implemented
    CHECK_EVAL("ISTEXT(INFO(\"recalc\"))",     Value(true));            //
    CHECK_EVAL("ISTEXT(INFO(\"release\"))",    Value(true));            //
//     CHECK_EVAL( "ISNUMBER(INFO(\"totmem\"))",   Value( true        ) ); // not implemented

    // TODO should ISTEXT return errorVALUE() if args is errorVALUE? false seems to be more logical
//     CHECK_EVAL( "ISTEXT(INFO(\"completely-unknown-category\"))", Value::errorVALUE()  ); // Error if the category is unknown
}

void TestInformationFunctions::testISBLANK()
{
    CHECK_EVAL("ISBLANK(1)",    Value(false));     // Numbers return false.
    CHECK_EVAL("ISBLANK(\"\")", Value(false));     // Text, even empty string, returns false.
    CHECK_EVAL("ISBLANK(B8)",   Value(true));      // Blank cell is true.
    CHECK_EVAL("ISBLANK(B7)",   Value(false));     // Non-blank cell is false.
}

void TestInformationFunctions::testISERR()
{
    CHECK_EVAL("ISERR(1/0)",      Value(true));      // Error values other than NA() return true.
    CHECK_EVAL("ISERR(NA())",     Value(false));     // NA() does NOT return True.
    CHECK_EVAL("ISERR(\"#N/A\")", Value(false));     // Text is not an error.
    CHECK_EVAL("ISERR(1)",        Value(false));     // Numbers are not an error.
}

void TestInformationFunctions::testISERROR()
{
    CHECK_EVAL("ISERROR(1/0)",      Value(true));      // Error values return true.
    CHECK_EVAL("ISERROR(NA())",     Value(true));      // Even NA().
    CHECK_EVAL("ISERROR(\"#N/A\")", Value(false));     // Text is not an error.
    CHECK_EVAL("ISERROR(1)",        Value(false));     // Numbers are not an error.
    CHECK_EVAL("ISERROR(CHOOSE(0; \"Apple\"; \"Orange\";"
               " \"Grape\"; \"Perry\"))", Value(true));    // If CHOOSE given
    // out-of-range value, ISERROR needs to capture it.
}

void TestInformationFunctions::testISEVEN()
{
    CHECK_EVAL("ISEVEN( 2)",   Value(true));        // 2 is even, because (2 modulo 2) = 0
    CHECK_EVAL("ISEVEN( 6)",   Value(true));        // 6 is even, because (6 modulo 2) = 0
    CHECK_EVAL("ISEVEN( 2.1)", Value(true));        //
    CHECK_EVAL("ISEVEN( 2.5)", Value(true));        //
    CHECK_EVAL("ISEVEN( 2.9)", Value(true));        // TRUNC(2.9)=2, and 2 is even.
    CHECK_EVAL("ISEVEN( 3)",   Value(false));       // 3 is not even.
    CHECK_EVAL("ISEVEN( 3.9)", Value(false));       // TRUNC(3.9)=3, and 3 is not even.
    CHECK_EVAL("ISEVEN(-2)",   Value(true));        //
    CHECK_EVAL("ISEVEN(-2.1)", Value(true));        //
    CHECK_EVAL("ISEVEN(-2.5)", Value(true));        //
    CHECK_EVAL("ISEVEN(-2.9)", Value(true));        // TRUNC(-2.9)=-2, and -2 is even.
    CHECK_EVAL("ISEVEN(-3)",   Value(false));       //
    CHECK_EVAL("ISEVEN(NA())", Value::errorNA());   //
    CHECK_EVAL("ISEVEN( 0)",   Value(true));        //
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETISEVEN(2.5)", Value(true)); // alternate function name
}

/*
void TestInformationFunctions::testISFORMULA()
{
    CHECK_EVAL( "ISFORMULA(B5)", Value( true  ) ); // Simple formulas that produce Number are still formulas
    CHECK_EVAL( "ISFORMULA(B3)", Value( true  ) ); // Simple formulas that produce Text are still formulas
    CHECK_EVAL( "ISFORMULA(C5)", Value( false ) ); // Cell constants are not formulas
    CHECK_EVAL( "ISFORMULA(C7)", Value( false ) ); // Cell constants are not formulas, even if they are dates
    CHECK_EVAL( "ISFORMULA(B9)", Value( true  ) ); // Formulas that return an error are still formulas
}
*/

void TestInformationFunctions::testISLOGICAL()
{
    CHECK_EVAL("ISLOGICAL(TRUE())",   Value(true));      // Logical values return true.
    CHECK_EVAL("ISLOGICAL(FALSE())",  Value(true));      // Logical values return true.
    CHECK_EVAL("ISLOGICAL(\"TRUE\")", Value(false));     // Text values are not logicals,
    // even if they can be converted.
}

void TestInformationFunctions::testISNONTEXT()
{
    CHECK_EVAL("ISNONTEXT(1)",      Value(true));      // Numbers are not text
    CHECK_EVAL("ISNONTEXT(TRUE())", Value(true));      // Logical values are not text.
    CHECK_EVAL("ISNONTEXT(\"1\")",  Value(false));     // TexText values are text, even
    // if they can be converted into a number.
    CHECK_EVAL("ISNONTEXT(B7)",     Value(false));     // B7 is a cell with text
    CHECK_EVAL("ISNONTEXT(B9)",     Value(true));      // B9 is an error, thus not text
    CHECK_EVAL("ISNONTEXT(B8)",     Value(true));      // B8 is a blank cell, so this will return TRUE
}

void TestInformationFunctions::testISNA()
{
    CHECK_EVAL("ISNA(1/0)",      Value(false));     // Error values other than NA() return False - the error does not propagate.
    CHECK_EVAL("ISNA(NA())",     Value(true));      // By definition
    // CHECK_EVAL( "ISNA(#N/A)",     Value( true  ) ); // By definition
    CHECK_EVAL("ISNA(\"#N/A\")", Value(false));     // Text is not NA
    CHECK_EVAL("ISNA(1)",        Value(false));     // Numbers are not NA
}

void TestInformationFunctions::testISNUMBER()
{
    CHECK_EVAL("ISNUMBER(1)",     Value(true));      // Numbers are numbers
    CHECK_EVAL("ISNUMBER(\"1\")", Value(false));     // Text values are not numbers,
    // even if they can be converted into a number.
}

void TestInformationFunctions::testISODD()
{
    CHECK_EVAL("ISODD(3)",    Value(true));      // 3 is odd, because (3 modulo 2) = 0
    CHECK_EVAL("ISODD(5)",    Value(true));      // 5 is odd, because (5 modulo 2) = 0
    CHECK_EVAL("ISODD(3.1)",  Value(true));      // TRUNC(3.1)=3, and 3 is odd
    CHECK_EVAL("ISODD(3.5)",  Value(true));      //
    CHECK_EVAL("ISODD(3.9)",  Value(true));      // TRUNC(3.9)=3, and 3 is odd.
    CHECK_EVAL("ISODD(4)",    Value(false));     // 4 is not even.
    CHECK_EVAL("ISODD(4.9)",  Value(false));     // TRUNC(4.9)=4, and 3 is not even.
    CHECK_EVAL("ISODD(-3)",   Value(true));      //
    CHECK_EVAL("ISODD(-3.1)", Value(true));      //
    CHECK_EVAL("ISODD(-3.5)", Value(true));      //
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETISODD(3.1)", Value(true)); // alternate function name
}

void TestInformationFunctions::testISTEXT()
{
    CHECK_EVAL("ISTEXT(1)",     Value(false));     // Numbers are not text
    CHECK_EVAL("ISTEXT(\"1\")", Value(true));      // Text values are text,
    // even if they can be converted into a number.
}

void TestInformationFunctions::testISREF()
{
    CHECK_EVAL("ISREF(B3)",     Value(true));        //
    CHECK_EVAL("ISREF(B3:C4)",  Value(true));        // The range operator produces references
    CHECK_EVAL("ISREF(1)",      Value(false));       // Numbers are not references
    CHECK_EVAL("ISREF(\"A1\")", Value(false));       // Text is not a reference, even if it looks a little like one
    CHECK_EVAL("ISREF(NA())",   Value::errorNA());   // Errors propagate through this function
}

void TestInformationFunctions::testN()
{
    CHECK_EVAL("N(6)",       Value(6));     // N does not change numbers.
    CHECK_EVAL("N(TRUE())",  Value(1));     // Does convert logicals.
    CHECK_EVAL("N(FALSE())", Value(0));     // Does convert logicals.
}

void TestInformationFunctions::testNA()
{
    CHECK_EVAL("ISERROR(NA())", Value(true));     // NA is an error.
    CHECK_EVAL("ISNA(NA())",    Value(true));     // Obviously, if this doesn't work, NA() or ISNA() is broken.
    CHECK_EVAL("ISNA(5+NA())",  Value(true));     // NA propagates through various functions
    // and operators, just like any other error type.
}

/*
void TestInformationFunctions::testNUMBERVALUE()
{
    CHECK_EVAL( "NUMBERVALUE(\"6\"      ; \".\")", Value(    6   ) ); // VALUE converts text to numbers (unlike N).
    CHECK_EVAL( "NUMBERVALUE(\"6,000.5\"; \".\")", Value( 6000.5 ) ); // Period works.
    CHECK_EVAL( "NUMBERVALUE(\"6.000,5\"; \",\")", Value( 6000.5 ) ); // Comma works
}
*/

// TODO row not working here
void TestInformationFunctions::testROW()
{
    CHECK_EVAL("ROW(B7)", Value(7));     // The second value of a cell reference is the row number.
//     CHECK_EVAL( "ROW()",   Value( 5 ) ); // Row of current cell is default, here formula in row 5.

    Value res(Value::Array);
    res.setElement(0, 0, Value(2));
    res.setElement(0, 1, Value(3));
    res.setElement(0, 2, Value(4));

    CHECK_EVAL("ROW(B2:B4)", res);      // Array with row numbers.
}

void TestInformationFunctions::testROWS()
{
    CHECK_EVAL("ROWS(C1)",      Value(1));      // Single cell range contains one row.
    CHECK_EVAL("ROWS(C1:C4)",   Value(4));      // Range with four rows.
    CHECK_EVAL("ROWS(A4:D100)", Value(97));     // Number of rows in range.
}

/*
void TestInformationFunctions::testSHEET()
{
    CHECK_EVAL( "SHEET(B7)>=1",         Value( true ) ); // If given, the sheet number of the reference is used.
    CHECK_EVAL( "SHEET(\"Sheet1\")>=1", Value( true ) ); // Given a sheet name, the sheet number is returned.
}

void TestInformationFunctions::testSHEETS()
{
    CHECK_EVAL( "SHEETS(B7)",  Value(    1 ) ); // If given, the sheet number of the reference is used.
    CHECK_EVAL( "SHEETS()>=1", Value( true ) ); // Range with four rows.
}
*/

void TestInformationFunctions::testTYPE()
{
    //  Value's Type | Type return
    // --------------+-------------
    //     Number    |     1
    //     Text      |     2
    //     Logical   |     4
    //     Error     |    16
    //     Array     |    64

    CHECK_EVAL("TYPE(1+2)",              Value(1));      // Number has TYPE code of 1
    CHECK_EVAL("TYPE(\"Hi\"&\"there\")", Value(2));      // Text has TYPE 2
    CHECK_EVAL("TYPE(NA())",             Value(16));     // Errors have TYPE 16.
}

void TestInformationFunctions::testVALUE()
{
    CHECK_EVAL("VALUE(\"6\")", Value(6));
    CHECK_EVAL("VALUE(\"1E5\")", Value(100000));
    CHECK_EVAL("VALUE(\"200%\")",  Value(2));
    CHECK_EVAL("VALUE(\"1.5\")", Value(1.5));
    // Check fractions
    CHECK_EVAL("VALUE(\"7 1/4\")", Value(7.25));
    CHECK_EVAL("VALUE(\"0 1/2\")", Value(0.5));
    CHECK_EVAL("VALUE(\"0 7/2\")", Value(3.5));
    CHECK_EVAL("VALUE(\"-7 1/5\")", Value(-7.2));
    CHECK_EVAL("VALUE(\"-7 10/50\")", Value(-7.2));
    CHECK_EVAL("VALUE(\"-7 10/500\")", Value(-7.02));
    CHECK_EVAL("VALUE(\"-7 4/2\")", Value(-9));
    CHECK_EVAL("VALUE(\"-7 40/20\")", Value(-9));
    // Check times
    CHECK_EVAL("VALUE(\"00:00\")", Value(0));
    CHECK_EVAL("VALUE(\"00:00:00\")", Value(0));
    CHECK_EVAL("VALUE(\"02:00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.0\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.000\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"2:03:05\") -2/24-3/(24*60) -5/(24*60*60)", Value(0));
    CHECK_EVAL("VALUE(\"2:03\")-(2/24)-(3/(24*60))", Value(0));
    // check dates - local dependent
    // CHECK_EVAL( "VALUE(\"5/21/06\")=DATE(2006;5;21)", Value( true ) );
    // CHECK_EVAL( "VALUE(\"1/2/2005\")=DATE(2005;1;2)", Value( true ) );
}

//
// cleanup test
//

void TestInformationFunctions::cleanupTestCase()
{
    delete m_doc;
}

QTEST_KDEMAIN(TestInformationFunctions, GUI)

#include "TestInformationFunctions.moc"
