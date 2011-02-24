/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Ariya Hidayat <ariya@kde.org>

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
#include "TestEngineeringFunctions.h"

#include "TestKspreadCommon.h"

void TestEngineeringFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation
#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),RoundNumber(y))
#define ROUND(x) (roundf(1e10 * x) / 1e10)

// round to get at most 10-digits number
static Value RoundNumber(double f)
{
    return Value(ROUND(f));
}

// round to get at most 10-digits number
static Value RoundNumber(const Value& v)
{
    if (v.isComplex()) {
        const double imag = numToDouble(v.asComplex().imag());
        QString complex = QString::number(ROUND(numToDouble(v.asComplex().real())), 'g', 10);
        if (imag >= 0.0)
            complex += '+';
        complex += QString::number(ROUND(imag), 'g', 10);
        complex += 'i';
        return Value(complex);
    } else if (v.isNumber())
        return Value(ROUND(numToDouble(v.asFloat())));
    else
        return v;
}

Value TestEngineeringFunctions::evaluate(const QString& formula)
{
    Formula f;
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

#if 0
    // this magically generates the CHECKs
    printf("  CHECK_EVAL( \"%s\",  %.14e) );\n", qPrintable(formula), result.asFloat());
#endif

    return RoundNumber(result);
}

void TestEngineeringFunctions::testBIN2DEC()
{
    // ODF-tests
    CHECK_EVAL("BIN2DEC(1010)", Value(10));
    CHECK_EVAL("BIN2DEC(11111)", Value(31));
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETBIN2DEC(1010)", Value(10));
}

void TestEngineeringFunctions::testBIN2OCT()
{
    // ODF-tests
    CHECK_EVAL("BIN2OCT(1010)", Value("12"));
    CHECK_EVAL("BIN2OCT(11111)", Value("37"));
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETBIN2OCT(1010)", Value("12"));
}

void TestEngineeringFunctions::testBIN2HEX()
{
    // ODF-tests
    CHECK_EVAL("BIN2HEX(1010)", Value("A"));
    CHECK_EVAL("BIN2HEX(11111)", Value("1F"));
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETBIN2HEX(1010)", Value("A"));
}

void TestEngineeringFunctions::testCOMPLEX()
{
    CHECK_EVAL("=IMREAL(COMPLEX(1;-3))", 1.0);
    CHECK_EVAL("=IMAGINARY(COMPLEX(0;-2))", -2.0);
//     CHECK_EVAL( "=IMAGINARY(COMPLEX(0;-2;\"i\"))", -2.0 );
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOMPLEX(1;-3))", 1.0);
}

void TestEngineeringFunctions::testCONVERT()
{
    CHECK_EVAL("=CONVERT(32;\"C\";\"F\")", Value(89.6));
//     CHECK_EVAL("=CONVERT(3;\"lbm\";\"kg\")", Value(1.3608));
//     CHECK_EVAL("=CONVERT(7.9;\"cal\";\"J\")", Value(33.0757));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCONVERT(32;\"C\";\"F\")", Value(89.6));
}

void TestEngineeringFunctions::testDEC2HEX()
{
    CHECK_EVAL("DEC2HEX(12)", Value("C"));
    CHECK_EVAL("DEC2HEX(55)", Value("37"));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDEC2HEX(12)", Value("C"));
}

void TestEngineeringFunctions::testDEC2BIN()
{
    CHECK_EVAL("DEC2BIN(12)", Value("1100"));
    CHECK_EVAL("DEC2BIN(55)", Value("110111"));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDEC2BIN(12)", Value("1100"));
}

void TestEngineeringFunctions::testDEC2OCT()
{
    CHECK_EVAL("DEC2OCT(12)", Value("14"));
    CHECK_EVAL("DEC2OCT(55)", Value("67"));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDEC2OCT(12)", Value("14"));
}

void TestEngineeringFunctions::testDELTA()
{
    CHECK_EVAL("DELTA(1.2; 3.4)", Value(0));
    CHECK_EVAL("DELTA(3;3)", Value(1));
//     CHECK_EVAL("DELTA(1;TRUE)", Value(1));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDELTA(1.2; 3.4)", Value(0));
}

void TestEngineeringFunctions::testERF()
{
    CHECK_EVAL("ERF(0.5)", Value(0.52049987781));
    CHECK_EVAL("ERF(0.1)", Value(0.1124629160));
    CHECK_EVAL("ABS(ERF(0.1; 0.5) - 0.40803696174) < 1e-6", Value(true));
    CHECK_EVAL("ABS(ERF(0.1) - ERF(0.5) + 0.40803696174) < 1e-6", Value(true));
    CHECK_EVAL("ABS(ERF(0.5; 0.1) + 0.40803696174) < 1e-6", Value(true));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETERF(0.5)", Value(0.52049987781));
}

void TestEngineeringFunctions::testERFC()
{
    CHECK_EVAL("ERFC(3)", Value(0.000022090497));
    CHECK_EVAL("ABS(ERFC(0.1)-(1-ERF(0.1))) < 1e-6", Value(true));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETERFC(3)", Value(0.000022090497));
}

void TestEngineeringFunctions::testGESTEP()
{
    CHECK_EVAL("=GESTEP(1.2; 3.4)", Value(0));
    CHECK_EVAL("=GESTEP(3; 3)", Value(1));
//     CHECK_EVAL("=GESTEP(0.4; TRUE)", Value(0));
    CHECK_EVAL("=GESTEP(4; 3)", Value(1));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETGESTEP(1.2; 3.4)", Value(0));
}

void TestEngineeringFunctions::testHEX2BIN()
{
    CHECK_EVAL("=HEX2BIN(\"a\")", Value("1010"));
    CHECK_EVAL("=HEX2BIN(37)", Value("110111"));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETHEX2BIN(\"a\")", Value("1010"));
}

void TestEngineeringFunctions::testHEX2DEC()
{
    CHECK_EVAL("=HEX2DEC(\"a\")", Value(10));
    CHECK_EVAL("=HEX2DEC(37)", Value(55));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETHEX2DEC(\"a\")", Value(10));
}

void TestEngineeringFunctions::testHEX2OCT()
{
    CHECK_EVAL("=HEX2OCT(\"a\")", Value("12"));
    CHECK_EVAL("=HEX2OCT(37)", Value("67"));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETHEX2OCT(\"a\")", Value("12"));
}
    
void TestEngineeringFunctions::testIMABS()
{
    CHECK_EVAL("=IMABS(COMPLEX(4;3))", 5.0);
    CHECK_EVAL("=IMABS(COMPLEX(4;-3))", 5.0);
    CHECK_EVAL("=IMABS(COMPLEX(-4;3))", 5.0);
    CHECK_EVAL("=IMABS(COMPLEX(-4;-3))", 5.0);
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMABS(COMPLEX(4;3))", 5.0);
}

void TestEngineeringFunctions::testIMAGINARY()
{
    CHECK_EVAL("=IMAGINARY(COMPLEX(4;-3))", -3.0);
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMAGINARY(COMPLEX(4;-3))", -3.0);
}

void TestEngineeringFunctions::testIMARGUMENT()
{
    CHECK_EVAL("=IMARGUMENT(COMPLEX(3;4))",    0.927295218001612);
    CHECK_EVAL("=IMARGUMENT(COMPLEX(3;-4))",  -0.927295218001612);
    CHECK_EVAL("=IMARGUMENT(COMPLEX(-3;4))",   2.214297435588180);
    CHECK_EVAL("=IMARGUMENT(COMPLEX(-3;-4))", -2.214297435588180);
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMARGUMENT(COMPLEX(3;4))",    0.927295218001612);
}

void TestEngineeringFunctions::testIMCONJUGATE()
{
    CHECK_EVAL("=IMABS(IMSUB(IMCONJUGATE(COMPLEX( 3; 4));COMPLEX( 3;-4)))", 0.0);
    CHECK_EVAL("=IMABS(IMSUB(IMCONJUGATE(COMPLEX(-3;-4));COMPLEX(-3;4)))", 0.0);
    CHECK_EVAL("=IMABS(IMSUB(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMCONJUGATE(COMPLEX( 3; 4));COMPLEX( 3;-4)))", 0.0);
}

void TestEngineeringFunctions::testIMCOS()
{
    CHECK_EVAL("=IMREAL   ( IMCOS(COMPLEX(1;1)) )",  0.833730025131149);
    CHECK_EVAL("=IMAGINARY( IMCOS(COMPLEX(1;1)) )", -0.988897705762865);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMCOS(COMPLEX(1;1)))", 0.833730025131149);
}

void TestEngineeringFunctions::testIMCOSH()
{
    CHECK_EVAL("=IMREAL   ( IMCOSH(COMPLEX(1;1)) )", 0.833730025131149);
    CHECK_EVAL("=IMAGINARY( IMCOSH(COMPLEX(1;1)) )", 0.988897705762865);
}

void TestEngineeringFunctions::testIMDIV()
{
    CHECK_EVAL("=IMREAL( IMDIV(COMPLEX(5;3); COMPLEX(1;-1)) )", 1.0);
    CHECK_EVAL("=IMAGINARY( IMDIV(COMPLEX(5;3); COMPLEX(1;-1)) )", 4.0);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMDIV(COMPLEX(5;3);COMPLEX(1;-1)))", 1.0);
}

void TestEngineeringFunctions::testIMEXP()
{
    CHECK_EVAL("=IMREAL( IMEXP(COMPLEX(1;2)) )",   -1.13120438375681);
    CHECK_EVAL("=IMAGINARY( IMEXP(COMPLEX(1;2)) )", 2.47172667200482);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMEXP(COMPLEX(1;2)))", -1.13120438375681);
}

void TestEngineeringFunctions::testIMLN()
{
    CHECK_EVAL("=IMREAL( IMLN(COMPLEX(1;2)) )",    0.80471895621705);
    CHECK_EVAL("=IMAGINARY( IMLN(COMPLEX(1;2)) )", 1.10714871779409);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMLN(COMPLEX(1;2)))", 0.80471895621705);
}

void TestEngineeringFunctions::testIMLOG10()
{
    CHECK_EVAL("=IMREAL(IMLOG10(COMPLEX(1;2)) )",     0.349485002168009);
    CHECK_EVAL("=IMAGINARY( IMLOG10(COMPLEX(1;2)) )", 0.480828578784234);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMLOG10(COMPLEX(1;2)) )", 0.349485002168009);
}

void TestEngineeringFunctions::testIMLOG2()
{
    CHECK_EVAL("=IMREAL( IMLOG2(COMPLEX(1;2)) )",   1.16096404744368);
    CHECK_EVAL("=IMAGINARY(IMLOG2(COMPLEX(1;2)) )", 1.59727796468811);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMLOG2(COMPLEX(1;2)) )", 1.16096404744368);
}

void TestEngineeringFunctions::testIMPOWER()
{
    CHECK_EVAL("=IMREAL( IMPOWER(COMPLEX(-1;2); 3) )", 11.0);
    CHECK_EVAL("=IMAGINARY( IMPOWER(COMPLEX(-1;2); 3) )", -2.0);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMPOWER(COMPLEX(-1;2);3))", 11.0);
}

void TestEngineeringFunctions::testIMPRODUCT()
{
    CHECK_EVAL("=IMREAL( IMPRODUCT(COMPLEX(1;2); COMPLEX(-1;-2); COMPLEX(2;-3) ))", -6.0);
    CHECK_EVAL("=IMAGINARY( IMPRODUCT(COMPLEX(1;2); COMPLEX(-1;-2); COMPLEX(2;-3) ))", -17.0);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMPRODUCT(COMPLEX(1;2);COMPLEX(-1;-2);COMPLEX(2;-3)))", -6.0);
}

void TestEngineeringFunctions::testIMREAL()
{
    CHECK_EVAL("=IMREAL(COMPLEX(4;-3))", 4.0);
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMREAL(COMPLEX(4;-3))", 4.0);
}

void TestEngineeringFunctions::testIMSIN()
{
    CHECK_EVAL("=IMREAL   ( IMSIN(COMPLEX(1;1)) )", 1.298457581415980);
    CHECK_EVAL("=IMAGINARY( IMSIN(COMPLEX(1;1)) )", 0.634963914784736);
    CHECK_EVAL("=IMREAL   ( COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMSIN(COMPLEX(1;1)) )", 1.298457581415980);
}

void TestEngineeringFunctions::testIMSINH()
{
    CHECK_EVAL("=IMREAL   ( IMSINH(COMPLEX(1;1)) )", 0.634963914784736);
    CHECK_EVAL("=IMAGINARY( IMSINH(COMPLEX(1;1)) )", 1.298457581415980);
}

void TestEngineeringFunctions::testIMSQRT()
{
    CHECK_EVAL("=IMREAL(IMSQRT(COMPLEX(1;-2)))",     1.272019649514070);
    CHECK_EVAL("=IMAGINARY(IMSQRT(COMPLEX(1;-2)))", -0.786151377757423);
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMSQRT(COMPLEX(1;-2)))", 1.272019649514070);
}

void TestEngineeringFunctions::testIMSUB()
{
    CHECK_EVAL("=IMREAL( IMSUB(COMPLEX(5;3); COMPLEX(3;2)) )", 2.0);
    CHECK_EVAL("=IMAGINARY( IMSUB(COMPLEX(5;3); COMPLEX(3;2)) )", 1.0);
    CHECK_EVAL("=IMREAL(IMSUB(\"5-3i\";\"2i\"))", 5.0);
    CHECK_EVAL("=IMAGINARY(IMSUB(\"5-3i\";\"2i\"))", -5.0);
    CHECK_EVAL("=IMREAL( COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMSUB(COMPLEX(5;3); COMPLEX(3;2)) )", 2.0);
}

void TestEngineeringFunctions::testIMSUM()
{
    CHECK_EVAL("=IMREAL(IMSUM(COMPLEX(1;2); COMPLEX(2;3) ))", 3.0);
    CHECK_EVAL("=IMAGINARY(IMSUM( COMPLEX(1;2); COMPLEX(2;3) ))", 5.0);
//     CHECK_EVAL( "=IMREAL( IMSUM(COMPLEX(1;2);B4:B5))", 6.0 );
//     CHECK_EVAL( "=IMAGINARY( IMSUM(COMPLEX(1;2);B4:B5))", 9.0 );
    CHECK_EVAL("=IMREAL(COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETIMSUM(COMPLEX(1;2); COMPLEX(2;3) ))", 3.0);
}

void TestEngineeringFunctions::testIMTAN()
{
    CHECK_EVAL("=IMREAL   ( IMTAN(COMPLEX(1;1)) )", 0.271752585319512);
    CHECK_EVAL("=IMAGINARY( IMTAN(COMPLEX(1;1)) )", 1.083923327338690);
}

void TestEngineeringFunctions::testIMTANH()
{
    CHECK_EVAL("=IMREAL   ( IMTANH(COMPLEX(1;1)) )", 1.083923327338690);
    CHECK_EVAL("=IMAGINARY( IMTANH(COMPLEX(1;1)) )", 0.271752585319512);
}

void TestEngineeringFunctions::testOCT2BIN()
{
    CHECK_EVAL("=OCT2BIN(\"12\")", Value("1010"));
    CHECK_EVAL("=OCT2BIN(\"55\")", Value("101101"));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETOCT2BIN(\"12\")", Value("1010"));
}

void TestEngineeringFunctions::testOCT2DEC()
{
    CHECK_EVAL("=OCT2DEC(\"12\")", Value(10));
    CHECK_EVAL("=OCT2DEC(\"55\")", Value(45));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETOCT2DEC(\"12\")", Value(10));
}

void TestEngineeringFunctions::testOCT2HEX()
{
    CHECK_EVAL("=OCT2HEX(\"12\")", Value("A"));
    CHECK_EVAL("=OCT2HEX(\"55\")", Value("2D"));
    CHECK_EVAL("=COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETOCT2HEX(\"12\")", Value("A"));
}
    
QTEST_KDEMAIN(TestEngineeringFunctions, GUI)

#include "TestEngineeringFunctions.moc"
