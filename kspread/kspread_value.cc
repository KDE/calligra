/* This file is part of the KDE project
   Copyright (C) 1998-2003 The KSpread Team
                           www.koffice.org/kspread

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

#include <qdatetime.h>
#include <qstring.h>
#include <qshared.h>

#include <kspread_value.h>

// helper class for KSpreadValue
class KSpreadValueData: public QShared
{
  public:

    KSpreadValue::Type type;

    // someday move to use union to reduce memory consumption
    bool b;
    int i;
    double f;
    QString s;

    // create empty data
    KSpreadValueData(): QShared(), type( KSpreadValue::Empty ),
      b( false ), i( 0 ), f( 0.0 ) { };

    // destroys data
    ~KSpreadValueData(){ if( this == s_null ){ s_null = 0; } }

    // static empty data to be shared
    static KSpreadValueData* null()
      { if( !s_null) s_null = new KSpreadValueData; else s_null->ref(); return s_null; }

    // decrease reference count
    void unref()
      {  QShared::deref(); if( !count ) delete this; }

    // true if it's null (which is shared)
    bool isNull(){ return this == s_null; }

  private:

    static KSpreadValueData* s_null;

    // don't use QShared::deref, use unref() instead
    void deref();
};

// to be shared between all empty value
KSpreadValueData* KSpreadValueData::s_null = 0;

// static things
KSpreadValue ks_value_empty;
KSpreadValue ks_error_div0;
KSpreadValue ks_error_na;
KSpreadValue ks_error_name;
KSpreadValue ks_error_null;
KSpreadValue ks_error_num;
KSpreadValue ks_error_ref;
KSpreadValue ks_error_value;

// create an empty value
KSpreadValue::KSpreadValue()
{
  d = KSpreadValueData::null();
}

// destructor
KSpreadValue::~KSpreadValue()
{
  d->unref();
}

// create value of certain type
KSpreadValue::KSpreadValue( KSpreadValue::Type _type )
{
  d = new KSpreadValueData;
  d->type = _type;
}

// copy constructor
KSpreadValue::KSpreadValue( const KSpreadValue& _value )
{
  d = KSpreadValueData::null();
  assign( _value );
}

// assignment operator
KSpreadValue& KSpreadValue::operator=( const KSpreadValue& _value )
{
  return assign( _value );
}

// create a boolean value
KSpreadValue::KSpreadValue( bool b )
{
  d = KSpreadValueData::null();
  setValue( b );
}

// create an integer value
KSpreadValue::KSpreadValue( int i )
{
  d = KSpreadValueData::null();
  setValue ( i );
}

// create a floating-point value
KSpreadValue::KSpreadValue( double f )
{
  d = KSpreadValueData::null();
  setValue( f );
}

// create a string value
KSpreadValue::KSpreadValue( const QString& s )
{
  d = KSpreadValueData::null();
  setValue( s );
}

// create a floating-point value from date/time
KSpreadValue::KSpreadValue( const QDateTime& dt )
{
  d = KSpreadValueData::null();
  setValue( dt );
}

// create a floating-point value from time
KSpreadValue::KSpreadValue( const QTime& dt )
{
  d = KSpreadValueData::null();
  setValue( dt );
}

// create a floating-point value from date
KSpreadValue::KSpreadValue( const QDate& dt )
{
  d = KSpreadValueData::null();
  setValue( dt );
}

// assign value from other
// shallow copy: only copy the data pointer
KSpreadValue& KSpreadValue::assign( const KSpreadValue& _value )
{
  d->unref();
  d = _value.d;
  d->ref();
  return *this;
}

// return type of the value
KSpreadValue::Type KSpreadValue::type() const
{
  return d ? d->type : Empty;
}

// set the value to boolean
void KSpreadValue::setValue( bool b )
{
  detach();
  d->type = Boolean;
  d->b = b;
}

// get the value as boolean
bool KSpreadValue::asBoolean() const
{
  bool result = false;

  if( type() == KSpreadValue::Boolean )
    result = d->b;

  return result;
}

// set the value to integer
void KSpreadValue::setValue( int i )
{
  detach();
  d->type = Integer;
  d->i = i;
}

// get the value as integer
int KSpreadValue::asInteger() const
{
  int result = 0;

  if( type() == KSpreadValue::Integer )
    result = d->i;

  if( type() == KSpreadValue::Float )
    result = static_cast<int>(d->f);

  return result;
}

void KSpreadValue::setValue( const KSpreadValue& v )
{
  assign( v );
}

// set the value as floating-point
void KSpreadValue::setValue( double f )
{
  detach();
  d->type = Float;
  d->f = f;
}

// get the value as floating-point
double KSpreadValue::asFloat() const
{
  double result = 0.0;

  if( type() == KSpreadValue::Float )
    result = d->f;

  if( type() == KSpreadValue::Integer )
    result = static_cast<double>(d->i);

  return result;
}

// set the value as string
void KSpreadValue::setValue( const QString& s )
{
  detach();
  d->type = String;
  d->s = s;
}

// get the value as string
QString KSpreadValue::asString() const
{
  QString result;

  if( type() == KSpreadValue::String )
    result = d->s;

  return result;
}

// set error message
void KSpreadValue::setError( const QString& msg )
{
  detach();
  d->type = Error;
  d->s = msg;
}

// get error message
QString KSpreadValue::errorMessage() const
{
  QString result;

  if( type() == KSpreadValue::Error )
    result = d->s;

  return result;
}

// set the value as date/time
// NOTE: date/time is stored as serial number
// Day 61 means 1st of March, 1900
void KSpreadValue::setValue( const QDateTime& dt )
{
  // reference is 31 Dec, 1899 midnight
  QDate refDate( 1899, 12, 31 );
  QTime refTime( 0, 0 );

  double f = refDate.daysTo( dt.date() ) + 1.0;
  f += refTime.secsTo( dt.time() ) / 86400.0;

  setValue( f );
}

void KSpreadValue::setValue( const QTime& time )
{
  // reference time is midnight
  QTime refTime( 0, 0 );
  double f = refTime.secsTo( time ) / 86400.0;

  setValue( f );
}

void KSpreadValue::setValue( const QDate& date )
{
  // reference date is 31 Dec, 1899
  QDate refDate = QDate( 1899, 12, 31 );
  double f = refDate.daysTo( date ) + 1.0;

  setValue( f );
}

// get the value as date/time
QDateTime KSpreadValue::asDateTime() const
{
  QDateTime dt = QDate( 1899, 12, 31 );

  double f = asFloat();
  dt = dt.addSecs( qRound( (f-(int)f) * 86400 ) );
  if( f > 1.0 ) dt = dt.addDays( (int) f-1 );
  
  return dt;
}

// reference to empty value
const KSpreadValue& KSpreadValue::empty()
{
  return ks_value_empty;
}

// reference to #DIV/0! error
const KSpreadValue& KSpreadValue::errorDIV0()
{
  if( !ks_error_div0.isError() )
    ks_error_div0.setError( "#DIV/0!" );
  return ks_error_div0;
}

// reference to #N/A error
const KSpreadValue& KSpreadValue::errorNA()
{
  if( !ks_error_na.isError() )
    ks_error_na.setError( "#N/A" );
  return ks_error_na;
}

// reference to #NAME? error
const KSpreadValue& KSpreadValue::errorNAME()
{
  if( !ks_error_name.isError() )
    ks_error_name.setError( "#NAME?" );
  return ks_error_name;
}

// reference to #NUM! error
const KSpreadValue& KSpreadValue::errorNUM()
{
  if( !ks_error_num.isError() )
    ks_error_num.setError( "#NUM!" );
  return ks_error_num;
}

// reference to #NULL! error
const KSpreadValue& KSpreadValue::errorNULL()
{
  if( !ks_error_null.isError() )
    ks_error_null.setError( "#NULL!" );
  return ks_error_null;
}

// reference to #REF! error
const KSpreadValue& KSpreadValue::errorREF()
{
  if( !ks_error_ref.isError() )
    ks_error_ref.setError( "#REF!" );
  return ks_error_ref;
}

// reference to #VALUE! error
const KSpreadValue& KSpreadValue::errorVALUE()
{
  if( !ks_error_value.isError() )
    ks_error_value.setError( "#VALUE!" );
  return ks_error_value;
}

// detach, create deep copy of KSpreadValueData
void KSpreadValue::detach()
{
  if( d->isNull() || ( d->count > 1 ) )
  {
    KSpreadValueData* n;
    n = new KSpreadValueData;

    n->type = d->type;
    switch( n->type )
    {
    case Empty: break;
    case Boolean: n->b = d->b; break;
    case Integer: n->i = d->i; break;
    case Float:   n->f = d->f; break;
    case String:  n->s = d->s; break;
    case Error:   n->s = d->s; break;
    default: break;
    }

    d->unref();
    d = n;
  }
}
