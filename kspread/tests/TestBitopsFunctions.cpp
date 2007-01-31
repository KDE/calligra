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

#include "TestBitopsFunctions.h"

#include <float.h> // DBL_EPSILON
#include <math.h>

using namespace KSpread;


// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestBitopsFunctions::evaluate(const QString& formula, Value& ex)
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

void TestBitopsFunctions::testBITAND()
{
    // basic check of all four bit combinations
    CHECK_EVAL( "BITAND(12;10)", Value( 8 ) );
    // test using an all-zero combo
    CHECK_EVAL( "BITAND(7;0)", Value( 0 ) );
    // test of 31-bit value
    CHECK_EVAL( "BITAND(2147483641; 2147483637)", Value( 2147483633 ) );
    // test of 32-bit value
    CHECK_EVAL( "BITAND(4294967289.0; 4294967285.0)", Value( (qint64) 4294967281 ) );
    // test of 32-bit value
    CHECK_EVAL( "BITAND(4294967289; 4294967285)", Value( (qint64) 4294967281 ) );

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

KSPREAD_TEST(TestBitopsFunctions)

#include "TestBitopsFunctions.moc"
