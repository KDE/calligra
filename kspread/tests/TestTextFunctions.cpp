/* This file is part of the KDE project
   Copyright 2007 Brad Hards <bradh@frogmouth.net>

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

#include "TestKspreadCommon.h"

#include "TestTextFunctions.h"


#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestTextFunctions::evaluate(const QString& formula, Value& ex)
{
    Formula f;
    QString expr = formula;
    if ( expr[0] != '=' )
        expr.prepend( '=' );
    f.setExpression( expr );
    Value result = f.eval();

    if(result.isFloat() && ex.isInteger())
        ex = Value(ex.asFloat());
    if(result.isInteger() && ex.isFloat())
        result = Value(result.asFloat());

    return result;
}

void TestTextFunctions::testCHAR()
{
    CHECK_EVAL( "CHAR(65)", Value( "A" ) );
    CHECK_EVAL( "CHAR(60)", Value( "<" ) );
    CHECK_EVAL( "CHAR(97)", Value( "a" ) );
    CHECK_EVAL( "CHAR(126)", Value( "~" ) );
    CHECK_EVAL( "CHAR(32)", Value( " " ) );

    // newline
    CHECK_EVAL( "LEN(CHAR(10))", Value( 1 ) );
    // number has to be >=0
    CHECK_EVAL( "CHAR(-1)", Value::errorNUM() );
}

void TestTextFunctions::testCLEAN()
{
    CHECK_EVAL( "CLEAN(\"Text\")", Value( "Text" ) );
    CHECK_EVAL( "CLEAN(CHAR(7)&\"Tex\"&CHAR(8)&\"t\"&CHAR(9))", Value( "Text" ) );
    CHECK_EVAL( "CLEAN(\"Hi there\")", Value( "Hi there" ) );
}

QTEST_KDEMAIN(TestTextFunctions, GUI)

#include "TestTextFunctions.moc"
