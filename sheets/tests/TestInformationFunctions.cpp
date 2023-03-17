// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestInformationFunctions.h"

#include <engine/CellBaseStorage.h>
#include <engine/Formula.h>
#include <engine/MapBase.h>
#include <engine/SheetBase.h>
#include <engine/CalculationSettings.h>
#include <engine/Localization.h>

#include "TestKspreadCommon.h"

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestInformationFunctions::evaluate(const QString& formula, Value& ex, const CellBase &cell)
{
    SheetBase* sheet = m_map->sheet(0);
    Formula f(sheet, cell);
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
    KLocalizedString::setApplicationDomain("calligrasheets");
    FunctionModuleRegistry::instance()->loadFunctionModules();
    m_map = new MapBase;
    // some tests are sensitive to locale, so use C for all tests
//    *(m_map->calculationSettings()->locale()) = KLocale("C", "C");

    m_map->addNewSheet();
    SheetBase* sheet = m_map->sheet(0);
    sheet->setSheetName("Sheet1");
    CellBaseStorage* storage = sheet->cellStorage();

    // FIXME
    const_cast<Localization*>(m_map->calculationSettings()->locale())->setLanguage("en_US");  // needed for decimals

    //
    // Test case data set
    //


     // A19:A31
     storage->setValue(1,19, Value(    1 ) );
     storage->setValue(1,20, Value(    2 ) );
     storage->setValue(1,21, Value(    4 ) );
     storage->setValue(1,22, Value(    8 ) );
     storage->setValue(1,23, Value(   16 ) );
     storage->setValue(1,24, Value(   32 ) );
     storage->setValue(1,25, Value(   64 ) );
     storage->setValue(1,26, Value(  128 ) );
     storage->setValue(1,27, Value(  256 ) );
     storage->setValue(1,28, Value(  512 ) );
     storage->setValue(1,29, Value( 1024 ) );
     storage->setValue(1,30, Value( 2048 ) );
     storage->setValue(1,31, Value( 4096 ) );

     // B1:B2
     Formula formula(sheet);
     formula.setExpression("=SUM(A19:A31)");
     storage->setFormula(2,1, formula);
     storage->setFormula(2,2, Formula::empty());

     // B3:B17
    storage->setValue(2, 3, Value("7"));
    storage->setValue(2, 4, Value(2));
    storage->setValue(2, 5, Value(3));
    storage->setValue(2, 6, Value(true));
    storage->setValue(2, 7, Value("Hello"));
     // B8 leave empty
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

     // C11:C17
     storage->setValue(3,11, Value( 5 ) );
     storage->setValue(3,12, Value( 6 ) );
     storage->setValue(3,13, Value( 8 ) );
     storage->setValue(3,14, Value( 4 ) );
     storage->setValue(3,15, Value( 3 ) );
     storage->setValue(3,16, Value( 2 ) );
     storage->setValue(3,17, Value( 1 ) );

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

    // A1000:G1000
    storage->setValue(1, 1000, Value("abc"));
    storage->setValue(2, 1000, Value("def"));
    storage->setValue(3, 1000, Value("efoob"));
    storage->setValue(4, 1000, Value("flka"));
    storage->setValue(5, 1000, Value("kde"));
    storage->setValue(6, 1000, Value("kde"));
    storage->setValue(7, 1000, Value("xxx"));

     // Z19:Z23
     storage->setValue(26,19, Value(   16 ) );
     storage->setValue(26,20, Value(    8 ) );
     storage->setValue(26,21, Value(    4 ) );
     storage->setValue(26,22, Value(    2 ) );
     storage->setValue(26,23, Value(    1 ) );

    // Add the second sheet
    m_map->addNewSheet();
    sheet = m_map->sheet(1);
    sheet->setSheetName("Sheet2");
    storage = sheet->cellStorage();

    // B1:B2
     Formula formula2(sheet);
     formula2.setExpression("=SUM(Sheet1!A19:Sheet1!A31)");
     storage->setFormula(2,1, formula2);
     storage->setFormula(2,2, Formula::empty());

    // Add the third sheet
    m_map->addNewSheet();
    sheet = m_map->sheet(2);
    sheet->setSheetName("Sheet3");
    storage = sheet->cellStorage();

    // A1:A2
     storage->setValue(1,1, Value( 1.1 ) );
     storage->setValue(1,2, Value( 2.2 ) );
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

void TestInformationFunctions::testCELL()
{
    CHECK_EVAL( "CELL(\"COL\";C7)", Value( 3 ) ); // Column C is column number 3.
    CHECK_EVAL( "CELL(\"COL\";Sheet2!C7)", Value( 3 ) );

    CHECK_EVAL( "CELL(\"ROW\";C7)", Value( 7 ) ); // Row 7 is row number 7.
    CHECK_EVAL( "CELL(\"ROW\";Sheet2!C7)", Value( 7 ) );

    CHECK_EVAL( "CELL(\"Sheet\";C7)", Value( 1 ) );
    CHECK_EVAL( "CELL(\"Sheet\";Sheet2!C7)", Value( 2 ) );
    CHECK_EVAL( "CELL(\"Sheet\";Sheet3!C7)", Value( 3 ) );

    CHECK_EVAL( "CELL(\"ADDRESS\";B7)", Value( "$B$7" ) );
    CHECK_EVAL( "CELL(\"ADDRESS\";Sheet2!B7)", Value( "'Sheet2'!$B$7" ) );

    Value v1( "$B$7" );
    Value r1 = evaluate("CELL(\"ADDRESS\")", v1, CellBase(m_map->sheet(0), 2, 7));
    QCOMPARE(r1, v1);

    Value v2( "$B$7" );
    Value r2 = evaluate("CELL(\"ADDRESS\")", v2, CellBase(m_map->sheet(1), 2, 7));
    QCOMPARE(r2, v2);

    //CHECK_EVAL( "CELL(\"ADDRESS\";'x:\\sample.ods'#Sheet3!B7)", Value( "'file:///x:/sample.ods'#$Sheet3.$B$7" ) );

    m_map->calculationSettings()->setFileName("/home/sample.ods");
    CHECK_EVAL( "CELL(\"FILENAME\")", Value( "/home/sample.ods" ) );
    CHECK_EVAL( "CELL(\"FILENAME\";B7)", Value( "/home/sample.ods" ) );
}

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
    // in a reference do not propagate the error into the result.
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

void TestInformationFunctions::testFORMULA()
{
    CHECK_EVAL( "FORMULA(B1)", Value( "=SUM(A19:A31)" ) ); // B1 contains a simple SUM formula
    CHECK_EVAL( "FORMULA(B2)", Value::errorNA() ); // Empty formula means no formula
    CHECK_EVAL( "FORMULA(B3)", Value::errorNA() ); // Cell constants are not formulas

    CHECK_EVAL( "LEN(FORMULA(B1))>0", Value( true ) ); // B7 is a formula, so this is fine and will produce a text value
}

void TestInformationFunctions::testINFO()
{
    CHECK_EVAL("INFO(\"recalc\")",             Value("Automatic"));     //
    CHECK_EVAL("ISTEXT(INFO(\"system\"))",     Value(true));            // The details of "system" vary by system, but it is always a text value
    CHECK_EVAL("ISTEXT(INFO(\"directory\"))",  Value(true));            // Test to see that every required category is supported
//     CHECK_EVAL( "ISNUMBER(INFO(\"memavail\"))", Value( true        ) ); // not implemented
//     CHECK_EVAL( "ISNUMBER(INFO(\"memused\"))",  Value( true        ) ); // not implemented
//    CHECK_EVAL("ISNUMBER(INFO(\"numfile\"))",  Value(true));             // not implemented
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

void TestInformationFunctions::testISFORMULA()
{
    CHECK_EVAL( "ISFORMULA(B1)", Value( true  ) ); // B1 contains a simple SUM formula
    CHECK_EVAL( "ISFORMULA(B2)", Value( false ) ); // Empty formula means no formula
    CHECK_EVAL( "ISFORMULA(B3)", Value( false ) ); // Cell constants are not formulas
}

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

void TestInformationFunctions::testSHEET()
{
    CHECK_EVAL("SHEET(B7)", Value(1));
    CHECK_EVAL("SHEET(Sheet2!B7)", Value(2));
    CHECK_EVAL("SHEET(Sheet3!B7)", Value(3));
    CHECK_EVAL("SHEET()", Value(1));
}

void TestInformationFunctions::testSHEETS()
{
    CHECK_EVAL( "SHEETS(B7)",  Value( 1 ) ); // If given, the sheet number of the reference is used.
    CHECK_EVAL( "SHEETS(Sheet1!B7:C9)",  Value( 1 ) );
    CHECK_EVAL( "SHEETS(Sheet1!A7:Sheet1!C9)",  Value( 1 ) );

    //TODO this should not fail! :-(
    //CHECK_EVAL( "SHEETS(Sheet1!B7:Sheet2!C9)",  Value( 2 ) );
    //CHECK_EVAL( "SHEETS(Sheet1!B7:Sheet3!C9)",  Value( 2 ) );
    //CHECK_EVAL( "SHEETS(Sheet1!A7:Sheet3!C9)",  Value( 3 ) );

    CHECK_EVAL( "SHEETS()", Value( 3 ) ); // Count all sheets
}

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
    CHECK_EVAL("VALUE(\"2:00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.0\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"02:00:00.000\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"2:00:00\")-2/24", Value(0));
    CHECK_EVAL("VALUE(\"2:03:05\") -2/24-3/(24*60) -5/(24*60*60)", Value(0));
    CHECK_EVAL("VALUE(\"2:03\")-(2/24)-(3/(24*60))", Value(0));
    // check dates - local dependent
    CHECK_EVAL( "VALUE(\"5/21/06\")=DATE(2006;5;21)", Value( true ) );
    CHECK_EVAL( "VALUE(\"1/2/2005\")=DATE(2005;1;2)", Value( true ) );
}

void TestInformationFunctions::testMATCH()
{
    // invalid matchType
    CHECK_EVAL("MATCH(1;A19:A31;\"foo\")", Value::errorVALUE());

    // matchType == 0, exact match
    CHECK_EVAL("MATCH(5;C11:C17;0)", Value(1));
    CHECK_EVAL("MATCH(8;C11:C17;0)", Value(3));
    CHECK_EVAL("MATCH(1;C11:C17;0)", Value(7));
    CHECK_EVAL("MATCH(13;C11:C17;0)", Value::errorNA());
    CHECK_EVAL("MATCH(5;C11:C11;0)", Value(1));
    CHECK_EVAL("MATCH(5;C11;0)", Value(1));
    CHECK_EVAL("MATCH(5;C11:D13;0)", Value::errorNA()); // not sure if this is the best error
    CHECK_EVAL("MATCH(\"Hello\";B3:B10;0)", Value(5));
    CHECK_EVAL("MATCH(\"hello\";B3:B10;0)", Value(5)); // match is always case insensitive
    CHECK_EVAL("MATCH(\"kde\";A1000:G1000;0)", Value(5));

    // matchType == 1 or omitted, largest value less than or equal to search value in sorted range
    CHECK_EVAL("MATCH(0;A19:A31;1)", Value::errorNA());
    CHECK_EVAL("MATCH(1;A19:A31;1)", Value(1));
    CHECK_EVAL("MATCH(16;A19:A31;1)", Value(5));
    CHECK_EVAL("MATCH(40;A19:A31;1)", Value(6));
    CHECK_EVAL("MATCH(4096;A19:A31;1)", Value(13));
    CHECK_EVAL("MATCH(5000;A19:A31;1)", Value(13));
    CHECK_EVAL("MATCH(\"aaa\";A1000:G1000)", Value::errorNA());
    CHECK_EVAL("MATCH(\"abc\";A1000:G1000)", Value(1));
    CHECK_EVAL("MATCH(\"efoob\";A1000:G1000)", Value(3));
    CHECK_EVAL("MATCH(\"epub\";A1000:G1000)", Value(3));
    CHECK_EVAL("MATCH(\"kde\";A1000:G1000)", Value(6));
    CHECK_EVAL("MATCH(\"xxx\";A1000:G1000)", Value(7));
    CHECK_EVAL("MATCH(\"zzz\";A1000:G1000)", Value(7));
    CHECK_EVAL("MATCH(13;C11:D13;1)", Value::errorNA()); // not sure if this is the best error

    // matchType == -1, smallest value greater than or equal to search value, in descending range
    CHECK_EVAL("MATCH(0;Z19:Z23;-1)", Value(5));
    CHECK_EVAL("MATCH(1;Z19:Z23;-1)", Value(5));
    CHECK_EVAL("MATCH(4;Z19:Z23;-1)", Value(3));
    CHECK_EVAL("MATCH(5;Z19:Z23;-1)", Value(2));
    CHECK_EVAL("MATCH(16;Z19:Z23;-1)", Value(1));
    CHECK_EVAL("MATCH(33;Z19:Z23;-1)", Value::errorNA());
    CHECK_EVAL("MATCH(13;C11:D13;-1)", Value::errorNA()); // not sure if this is the best error
}

//
// cleanup test
//

void TestInformationFunctions::cleanupTestCase()
{
    delete m_map;
}

QTEST_MAIN(TestInformationFunctions)
