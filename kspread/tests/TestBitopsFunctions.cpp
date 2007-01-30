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

#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),RoundNumber( y ))

// round to get at most 15-digits number
static Value RoundNumber(double f)
{
  return Value( QString::number(f, 'g', 15) );
}

static Value RoundNumber(const Value& v)
{
  if(v.isNumber())
  {
    double d = v.asFloat();
    if(fabs(d) < DBL_EPSILON)
      d = 0.0;
    return Value( QString::number(d, 'g', 15) );
  }
  else
    return v;
}

Value TestBitopsFunctions::evaluate(const QString& formula)
{
  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

#if 0
  // this magically generates the CHECKs
  printf("  CHECK_EVAL( \"%s\",  %15g) );\n", qPrintable(formula), result.asFloat());
#endif

  return RoundNumber(result);
}
namespace QTest
{
  template<>
  char *toString(const Value& value)
  {
    QString message;
    QTextStream ts( &message, QIODevice::WriteOnly );
    if( value.isFloat() )
      ts << QString::number(value.asFloat(), 'g', 20);
    else
      ts << value;
    return qstrdup(message.toLatin1());
  }
}

void TestBitopsFunctions::testBITAND()
{
    // basic check of all four bit combinations
    CHECK_EVAL( "BITAND(12;10)", 8 );
    // test using an all-zero combo
    CHECK_EVAL( "BITAND(7;0)",  0 );
    // test of 31-bit value
    CHECK_EVAL( "BITAND(2147483641; 2147483637)", 2147483633 );
    // test of 32-bit value
    CHECK_EVAL( "BITAND(4294967289.0; 4294967285.0)", 4294967281 );
    // test of 32-bit value
    CHECK_EVAL( "BITAND(4294967289; 4294967285)", 4294967281 );

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
