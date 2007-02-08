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

#include "qtest_kde.h"

#include <Formula.h>
#include <Value.h>

#include "TestLogicFunctions.h"

#include <float.h> // DBL_EPSILON
#include <math.h>

using namespace KSpread;


// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestLogicFunctions::evaluate(const QString& formula, Value& ex)
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

namespace QTest
{
  template<>
  char *toString(const Value& value)
  {
    QString message;
    QTextStream ts( &message, QIODevice::WriteOnly );
    ts << value;
    return qstrdup(message.toLatin1());
  }
}

void TestLogicFunctions::testAND()
{
    CHECK_EVAL( "AND(FALSE();FALSE())", Value( false ) );
    CHECK_EVAL( "AND(FALSE();TRUE())", Value( false ) );
    CHECK_EVAL( "AND(TRUE();FALSE())", Value( false ) );
    CHECK_EVAL( "AND(TRUE();TRUE())", Value( true ) );
    // errors propogate
    CHECK_EVAL( "AND(TRUE();NA())", Value::errorNA() );
    CHECK_EVAL( "AND(NA();TRUE())", Value::errorNA() );
    // Nonzero considered TRUE
    CHECK_EVAL( "AND(1;TRUE())", Value( true ) );
    CHECK_EVAL( "AND(2;TRUE())", Value( true ) );
    // zero considered false
    CHECK_EVAL( "AND(0;TRUE())", Value( false ) );
    // multiple parameters...
    CHECK_EVAL( "AND(TRUE();TRUE();TRUE())", Value( true ) );
    CHECK_EVAL( "AND(TRUE();TRUE();FALSE())", Value( false ) );
    CHECK_EVAL( "AND(FALSE();TRUE();TRUE())", Value( false ) );
    CHECK_EVAL( "AND(TRUE();FALSE();TRUE())", Value( false ) );
    CHECK_EVAL( "AND(TRUE();FALSE();FALSE())", Value( false ) );
    // single parameter
    CHECK_EVAL( "AND(TRUE())", Value( true ) );
    CHECK_EVAL( "AND(FALSE())", Value( false ) );
}


void TestLogicFunctions::testOR()
{
    CHECK_EVAL( "OR(FALSE();FALSE())", Value( false ) );
    CHECK_EVAL( "OR(FALSE();TRUE())", Value( true ) );
    CHECK_EVAL( "OR(TRUE();FALSE())", Value( true ) );
    CHECK_EVAL( "OR(TRUE();TRUE())", Value( true ) );
    // errors propogate
    CHECK_EVAL( "OR(TRUE();NA())", Value::errorNA() );
    CHECK_EVAL( "OR(NA();TRUE())", Value::errorNA() );
    // Nonzero considered TRUE
    CHECK_EVAL( "OR(1;TRUE())", Value( true ) );
    CHECK_EVAL( "OR(2;TRUE())", Value( true ) );
    // zero considered false
    CHECK_EVAL( "OR(0;TRUE())", Value( true ) );
    CHECK_EVAL( "OR(0;1)", Value( true ) );
    CHECK_EVAL( "OR(0;0)", Value( false ) );
    // multiple parameters...
    CHECK_EVAL( "OR(TRUE();TRUE();TRUE())", Value( true ) );
    CHECK_EVAL( "OR(FALSE();FALSE();FALSE())", Value( false ) );
    CHECK_EVAL( "OR(TRUE();TRUE();FALSE())", Value( true ) );
    CHECK_EVAL( "OR(FALSE();TRUE();TRUE())", Value( true ) );
    CHECK_EVAL( "OR(TRUE();FALSE();TRUE())", Value( true ) );
    CHECK_EVAL( "OR(TRUE();FALSE();FALSE())", Value( true ) );
    // single parameter
    CHECK_EVAL( "OR(TRUE())", Value( true ) );
    CHECK_EVAL( "OR(FALSE())", Value( false ) );
}

void TestLogicFunctions::testXOR()
{
    CHECK_EVAL( "XOR(FALSE();FALSE())", Value( false ) );
    CHECK_EVAL( "XOR(FALSE();TRUE())", Value( true ) );
    CHECK_EVAL( "XOR(TRUE();FALSE())", Value( true ) );
    CHECK_EVAL( "XOR(TRUE();TRUE())", Value( false ) );
    // errors propogate
    CHECK_EVAL( "XOR(TRUE();NA())", Value::errorNA() );
    CHECK_EVAL( "XOR(NA();TRUE())", Value::errorNA() );
    CHECK_EVAL( "XOR(FALSE();NA())", Value::errorNA() );
    CHECK_EVAL( "XOR(NA();FALSE())", Value::errorNA() );
    // Nonzero considered TRUE
    CHECK_EVAL( "XOR(1;TRUE())", Value( false ) );
    CHECK_EVAL( "XOR(3;4)", Value( false ) );
    CHECK_EVAL( "XOR(2;TRUE())", Value( false ) );
    CHECK_EVAL( "XOR(FALSE();1)", Value( true ) );
    CHECK_EVAL( "XOR(2;FALSE())", Value( true ) );
    // zero considered false
    CHECK_EVAL( "XOR(0;TRUE())", Value( true ) );
    CHECK_EVAL( "XOR(0;1)", Value( true ) );
    CHECK_EVAL( "XOR(0;0)", Value( false ) );
    // multiple parameters...
    CHECK_EVAL( "XOR(TRUE();TRUE();TRUE())", Value( false ) );
    CHECK_EVAL( "XOR(FALSE();FALSE();FALSE())", Value( false ) );
    CHECK_EVAL( "XOR(TRUE();TRUE();FALSE())", Value( false ) );
    CHECK_EVAL( "XOR(FALSE();TRUE();TRUE())", Value( false) );
    CHECK_EVAL( "XOR(TRUE();FALSE();TRUE())", Value( false ) );
    CHECK_EVAL( "XOR(TRUE();FALSE();FALSE())", Value( true ) );
    CHECK_EVAL( "XOR(FALSE();FALSE();TRUE())", Value( true ) );
    CHECK_EVAL( "XOR(FALSE();FALSE();TRUE();FALSE())", Value( true ) );
    // single parameter
    CHECK_EVAL( "XOR(TRUE())", Value( true ) );
    CHECK_EVAL( "XOR(FALSE())", Value( false ) );
}

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kapplication.h>

#define KSPREAD_TEST(TestObject) \
int main(int argc, char *argv[]) \
{ \
    setenv("LC_ALL", "C", 1); \
    setenv("KDEHOME", QFile::encodeName( QDir::homePath() + "/.kde-unit-test" ), 1); \
    KAboutData aboutData( "qttest", "qttest", "version" );  \
    KCmdLineArgs::init(&aboutData); \
    KApplication app; \
    TestObject tc; \
    return QTest::qExec( &tc, argc, argv ); \
}

KSPREAD_TEST(TestLogicFunctions)

#include "TestLogicFunctions.moc"
