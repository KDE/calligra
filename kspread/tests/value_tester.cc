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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tester.h"
#include "value_tester.h"

#include <math.h>

#include <kspread_value.h>

#define CHECK(x,y)  check(#x,x,y)

using namespace KSpread;

ValueTester::ValueTester(): Tester()
{
}

QString ValueTester::name()
{
  return QString("Value");
}

template<typename T>
void ValueTester::check( const char* msg, const T& result, const T& expected )
{
  testCount++;
  if( result != expected )
  {
    QString msg;
    QTextStream ts( &msg, IO_WriteOnly );
    ts << "Result:";
    ts << result;
    ts << ", ";
    ts << "Expected:";
    ts << expected;
    fail( __FILE__, __LINE__, msg );
  }
}

void ValueTester::check( const char* msg, bool result, bool expected )
{
  testCount++;
  if( result != expected )
  {
    QString msg;
    QTextStream ts( &msg, IO_WriteOnly );
    ts << "Result: ";
    if( result ) ts << "True"; else "False";
    ts << ", ";
    ts << "Expected: ";
    if( expected ) ts << "True"; else "False";
    fail( __FILE__, __LINE__, msg );
  }
}

void ValueTester::run()
{
  testCount = 0;
  errorList.clear();

  KSpreadValue* v1;
  KSpreadValue* v2;
  
  // empty value
  v1 = new KSpreadValue();
  CHECK( v1->type(), KSpreadValue::Empty );
  delete v1;

  // boolean value (true)
  v1 = new KSpreadValue( true );
  CHECK( v1->type(), KSpreadValue::Boolean );
  CHECK( v1->asBoolean(), true );
  v1->setValue( 1 ); // dummy
  v1->setValue( true );
  CHECK( v1->type(), KSpreadValue::Boolean );
  CHECK( v1->asBoolean(), true );
  delete v1;
  
  // boolean value (false)
  v1 = new KSpreadValue( false );
  CHECK( v1->type(), KSpreadValue::Boolean );
  CHECK( v1->asBoolean(), false );
  v1->setValue( 4 ); // dummy
  v1->setValue( false );
  CHECK( v1->type(), KSpreadValue::Boolean );
  CHECK( v1->asBoolean(), false );
  delete v1;

  // integer value
  v1 = new KSpreadValue( 1977 );
  CHECK( v1->type(), KSpreadValue::Integer );
  CHECK( v1->asInteger(), (long)1977 );
  v1->setValue( false ); // dummy
  v1->setValue( 14 );
  CHECK( v1->type(), KSpreadValue::Integer );
  CHECK( v1->isInteger(), true );
  CHECK( v1->isFloat(), false );
  CHECK( v1->isString(), false );
  CHECK( v1->isNumber(), true );
  CHECK( v1->asInteger(), (long)14 );
  delete v1;
  
  // floating-point value
  v1 = new KSpreadValue( M_PI );
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v1->asFloat(), M_PI );
  v1->setValue( false ); // dummy
  v1->setValue( 14.03 );
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v1->isInteger(), false );
  CHECK( v1->isFloat(), true );
  CHECK( v1->isString(), false );
  CHECK( v1->isNumber(), true );
  CHECK( v1->asFloat(), 14.03 );
  delete v1;
  
  // string value
  v1 = new KSpreadValue( QString("Ailinon" ) );
  CHECK( v1->type(), KSpreadValue::String );
  CHECK( v1->asString(), QString("Ailinon" ) );
  v1->setValue( 7 ); // dummy
  v1->setValue( QString("spreadsheet" ) );
  CHECK( v1->type(), KSpreadValue::String );
  CHECK( v1->isInteger(), false );
  CHECK( v1->isFloat(), false );
  CHECK( v1->isString(), true );
  CHECK( v1->isNumber(), false );
  CHECK( v1->asString(), QString("spreadsheet" ) );
  delete v1;

  // TODO date value

  // time value
  v1 = new KSpreadValue();
  v1->setValue( QTime( 0, 0, 0 ) );
  CHECK( v1->type(), KSpreadValue::Float );
  int time_error = 0; // used to  save time, bail on first error
  for( unsigned h = 0; !time_error && h < 24; h++ )
  for( unsigned m = 0; !time_error && m < 60; m++ )
  for( unsigned s = 0; !time_error && s < 60; s++ )
  {
    QTime t1 = QTime( h, m, s );
    v1->setValue( t1 );
    QTime t2 = v1->asTime();
    if( t1.hour() != t2.hour() ) time_error++;
    if( t1.minute() != t2.minute() ) time_error++;
    if( t1.second() != t2.second() ) time_error++;
    if( t1.msec() != t2.msec() ) time_error++;
  }
  CHECK( time_error, 0 );
  delete v1;

  // time value (msec)
  v1 = new KSpreadValue();
  v1->setValue( QTime( 0, 0, 0 ) );
  CHECK( v1->type(), KSpreadValue::Float );
  int msec_error = 0; // used to  save time, bail on first error
  for( unsigned ms= 0;ms < 1000;ms++ )
  {
    QTime t1 = QTime( 1, 14, 2, ms );
    v1->setValue( t1 );
    QTime t2 = v1->asTime();
    if( t1.hour() != t2.hour() ) msec_error++;
    if( t1.minute() != t2.minute() ) msec_error++;
    if( t1.second() != t2.second() ) msec_error++;
    if( t1.msec() != t2.msec() ) msec_error++;
    if( msec_error ) break;
  }
  CHECK( msec_error, 0 );
  delete v1;
  
    
  // TODO error values
  
  // TODO compare values
  // TODO add, sub, mul, div values
  // TODO pow
  
  // array
  v1 = new KSpreadValue( 10, 3 ); // 10 columns, 3 rows
  CHECK( v1->type(), KSpreadValue::Array );
  CHECK( v1->columns(), (unsigned)10 );
  CHECK( v1->rows(), (unsigned)3 );
  delete v1;
  
  // check empty value in array
  v1 = new KSpreadValue( 1, 1 );
  CHECK( v1->type(), KSpreadValue::Array );
  v2 = new KSpreadValue( v1->element( 0, 0 ) );
  CHECK( v2->type(), KSpreadValue::Empty );
  delete v1;
  
  // fill simple 1x1 array
  v1 = new KSpreadValue( 1, 1 );
  CHECK( v1->type(), KSpreadValue::Array );
  v2 = new KSpreadValue( 14.3 );
  v1->setElement( 0, 0, *v2 );
  delete v2;
  v2 = new KSpreadValue( v1->element( 0, 0 ) );
  CHECK( v2->type(), KSpreadValue::Float );
  CHECK( v2->asFloat(), 14.3 );
  delete v2;
  delete v1;
  
  // stress test, array of 1000x1000
  v1 = new KSpreadValue( 1000, 1000 );
  CHECK( v1->type(), KSpreadValue::Array );
  for( unsigned c=0; c<1000; c++ )
  for( unsigned r=0; r<1000; r++ )
  {
    int index = 1000*r + c;
    v1->setElement( c, r, KSpreadValue( index ) );
  }
  int array_error = 0;
  for( unsigned c=0; !array_error && c<1000; c++ )
  for( unsigned r=0; !array_error && r<1000; r++ )
  {
    int index = 1000*r + c;
    v2 = new KSpreadValue( v1->element( c, r ) );
    if( v2->type() != KSpreadValue::Integer ) array_error++;
    if( v2->asInteger() != index ) array_error++;
    delete v2;
  }
  CHECK( array_error, (int)0 );
  delete v1;
  
  // assignment of array value
  v1 = new KSpreadValue( 1, 1 );
  CHECK( v1->type(), KSpreadValue::Array );
  v1->setElement( 0, 0, KSpreadValue( 14.3) );
  v2 = new KSpreadValue( *v1 ); // v2 is now also an array
  delete v1;
  v1 = new KSpreadValue( v2->element( 0, 0 ) );
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v1->asFloat(), 14.3 );
  delete v1;
  delete v2;
    
  // copy value
  v1 = new KSpreadValue();
  v1->setValue( 14.3 );
  v2 = new KSpreadValue( *v1 );
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v2->type(), KSpreadValue::Float );
  CHECK( v1->asFloat(), 14.3 );
  CHECK( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
  
  // value assignment
  v1 = new KSpreadValue( 14.3 );
  v2 = new KSpreadValue( true );
  v2->assign( *v1 );
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v2->type(), KSpreadValue::Float );
  CHECK( v1->asFloat(), 14.3 );
  CHECK( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
  
  // verify detachment
  v1 = new KSpreadValue( 14.3 );
  v2 = new KSpreadValue( *v1 );
  v2->detach(); // v1 and v2 don't share data anymore
  CHECK( v1->type(), KSpreadValue::Float );
  CHECK( v2->type(), KSpreadValue::Float );
  CHECK( v1->asFloat(), 14.3 );
  CHECK( v2->asFloat(), 14.3 );
  delete v1;
  delete v2;
}
