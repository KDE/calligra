/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <math.h>

#include "qtest_kde.h"

#include <Doc.h>
#include <Value.h>

#include "valuetest.h"

using namespace KSpread;

void ValueTester::testEmpty()
{
  Value* v1;

  // empty value
  v1 = new Value();
  QCOMPARE( v1->type(), Value::Empty );
  delete v1;
}

void ValueTester::testBoolean()
{
  Value* v1;

  // boolean value (true)
  v1 = new Value( true );
  QCOMPARE( v1->type(), Value::Boolean );
  QCOMPARE( v1->asBoolean(), true );
  v1->setValue( 1 ); // dummy
  v1->setValue( true );
  QCOMPARE( v1->type(), Value::Boolean );
  QCOMPARE( v1->asBoolean(), true );
  delete v1;

  // boolean value (false)
  v1 = new Value( false );
  QCOMPARE( v1->type(), Value::Boolean );
  QCOMPARE( v1->asBoolean(), false );
  v1->setValue( 4 ); // dummy
  v1->setValue( false );
  QCOMPARE( v1->type(), Value::Boolean );
  QCOMPARE( v1->asBoolean(), false );
  delete v1;
}

void ValueTester::testInteger()
{
  Value* v1;

  // integer value
  v1 = new Value( 1977 );
  QCOMPARE( v1->type(), Value::Integer );
  QCOMPARE( v1->asInteger(), (long)1977 );
  v1->setValue( false ); // dummy
  v1->setValue( 14 );
  QCOMPARE( v1->type(), Value::Integer );
  QCOMPARE( v1->isInteger(), true );
  QCOMPARE( v1->isFloat(), false );
  QCOMPARE( v1->isString(), false );
  QCOMPARE( v1->isNumber(), true );
  QCOMPARE( v1->asInteger(), (long)14 );
  delete v1;
}

void ValueTester::testFloat()
{
  Value* v1;

  // floating-point value
  v1 = new Value( M_PI );
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v1->asFloat(), M_PI );
  v1->setValue( false ); // dummy
  v1->setValue( 14.03 );
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v1->isInteger(), false );
  QCOMPARE( v1->isFloat(), true );
  QCOMPARE( v1->isString(), false );
  QCOMPARE( v1->isNumber(), true );
  QCOMPARE( v1->asFloat(), 14.03 );
  delete v1;
}

void ValueTester::testString()
{
  Value* v1;

  // string value
  v1 = new Value( QString("Ailinon" ) );
  QCOMPARE( v1->type(), Value::String );
  QCOMPARE( v1->asString(), QString("Ailinon" ) );
  v1->setValue( 7 ); // dummy
  v1->setValue( QString("spreadsheet" ) );
  QCOMPARE( v1->type(), Value::String );
  QCOMPARE( v1->isInteger(), false );
  QCOMPARE( v1->isFloat(), false );
  QCOMPARE( v1->isString(), true );
  QCOMPARE( v1->isNumber(), false );
  QCOMPARE( v1->asString(), QString("spreadsheet" ) );
  delete v1;
}

void ValueTester::testDate()
{
  Value* v1;

  // check all (valid) dates from 1900 to 2050
  // note: bail on first error immediately
  Doc doc;
  QDate refDate( 1899, 12, 31 );
  v1 = new Value();
  bool date_error = 0;
  for( unsigned y = 1900; !date_error && y < 2050; y++ )
  for( unsigned m = 1; !date_error && m <= 12; m++ )
  for( unsigned d = 1; !date_error && d <= 31; d++ )
  {
    QDate dv1 = QDate( y, m, d );
    if( !dv1.isValid() ) continue;
    double serialNo = -dv1.daysTo( refDate ) + 1.0;
    v1->setValue( Value( dv1, &doc ) );
    QCOMPARE(v1->asFloat(),serialNo);
    date_error = v1->asFloat() != serialNo;
  }
  delete v1;
}

void ValueTester::testTime()
{
  Doc doc;
  Value* v1;

  // time value
  v1 = new Value();
  v1->setValue( Value( QTime( 0, 0, 0 ), &doc ) );
  QCOMPARE( v1->type(), Value::Float );
  for( unsigned h = 0; h < 24; h++ )
  for( unsigned m = 0; m < 60; m++ )
  for( unsigned s = 0; s < 60; s++ )
  {
    QTime t1 = QTime( h, m, s );
    v1->setValue( Value( t1, &doc ) );
    QTime t2 = v1->asTime( &doc );
    QCOMPARE( t1.hour(), t2.hour() );
    QCOMPARE( t1.minute(), t2.minute() );
    QCOMPARE( t1.second(), t2.second() );
    QCOMPARE( t1.msec(), t2.msec() );
  }
  delete v1;

  // time value (msec)
  v1 = new Value();
  v1->setValue( Value( QTime( 0, 0, 0 ), &doc ) );
  QCOMPARE( v1->type(), Value::Float );
  for( unsigned ms= 0;ms < 1000;ms++ )
  {
    QTime t1 = QTime( 1, 14, 2, ms );
    v1->setValue( Value( t1, &doc ) );
    QTime t2 = v1->asTime( &doc );
    QCOMPARE( t1.hour(), t2.hour() );
    QCOMPARE( t1.minute(), t2.minute() );
    QCOMPARE( t1.second(), t2.second() );
    QCOMPARE( t1.msec(), t2.msec() );
  }
  delete v1;
}

void ValueTester::testError()
{
  // TODO error values

  // TODO compare values
  // TODO add, sub, mul, div values
  // TODO pow
}

void ValueTester::testArray()
{
  Value* v1;
  Value* v2;

  // array
  v1 = new Value( 10, 3 ); // 10 columns, 3 rows
  QCOMPARE( v1->type(), Value::Array );
  QCOMPARE( v1->columns(), (unsigned)10 );
  QCOMPARE( v1->rows(), (unsigned)3 );
  delete v1;

  // check empty value in array
  v1 = new Value( 1, 1 );
  QCOMPARE( v1->type(), Value::Array );
  v2 = new Value( v1->element( 0, 0 ) );
  QCOMPARE( v2->type(), Value::Empty );
  delete v1;

  // fill simple 1x1 array
  v1 = new Value( 1, 1 );
  QCOMPARE( v1->type(), Value::Array );
  v2 = new Value( 14.3 );
  v1->setElement( 0, 0, *v2 );
  delete v2;
  v2 = new Value( v1->element( 0, 0 ) );
  QCOMPARE( v2->type(), Value::Float );
  QCOMPARE( v2->asFloat(), 14.3 );
  delete v2;
  delete v1;

  // stress test, array of 1000x1000
  v1 = new Value( 1000, 1000 );
  QCOMPARE( v1->type(), Value::Array );
  for( unsigned c=0; c<1000; c++ )
  for( unsigned r=0; r<1000; r++ )
  {
    int index = 1000*r + c;
    v1->setElement( c, r, Value( index ) );
  }
  int array_error = 0;
  for( unsigned c=0; !array_error && c<1000; c++ )
  for( unsigned r=0; !array_error && r<1000; r++ )
  {
    int index = 1000*r + c;
    v2 = new Value( v1->element( c, r ) );
    if( v2->type() != Value::Integer ) array_error++;
    if( v2->asInteger() != index ) array_error++;
    delete v2;
  }
  QCOMPARE( array_error, (int)0 );
  delete v1;

  // assignment of array value
  v1 = new Value( 1, 1 );
  QCOMPARE( v1->type(), Value::Array );
  v1->setElement( 0, 0, Value( 14.3) );
  v2 = new Value( *v1 ); // v2 is now also an array
  delete v1;
  v1 = new Value( v2->element( 0, 0 ) );
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v1->asFloat(), 14.3 );
  delete v1;
  delete v2;
}

void ValueTester::testCopy()
{
  Value* v1;
  Value* v2;

  // copy value
  v1 = new Value();
  v1->setValue( 14.3 );
  v2 = new Value( *v1 );
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v2->type(), Value::Float );
  QCOMPARE( v1->asFloat(), 14.3 );
  QCOMPARE( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
}

void ValueTester::testAssignment()
{
  Value* v1;
  Value* v2;

  // value assignment
  v1 = new Value( 14.3 );
  v2 = new Value( true );
  v2->assign( *v1 );
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v2->type(), Value::Float );
  QCOMPARE( v1->asFloat(), 14.3 );
  QCOMPARE( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
}

void ValueTester::testDetach()
{
  Value* v1;
  Value* v2;

  // verify detachment
  v1 = new Value( 14.3 );
  v2 = new Value( *v1 );
  v2->detach(); // v1 and v2 don't share data anymore
  QCOMPARE( v1->type(), Value::Float );
  QCOMPARE( v2->type(), Value::Float );
  QCOMPARE( v1->asFloat(), 14.3 );
  QCOMPARE( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
}

QTEST_KDEMAIN(ValueTester, GUI)
#include "valuetest.moc"
