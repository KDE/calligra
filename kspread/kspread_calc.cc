/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

// We have to include this because Qt is shit
//#include <qprinter.h>
#include "kspread_doc.h"

#include <iostream>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <qlist.h>
#include <qstring.h>
#include <stdlib.h>
#include <assert.h>

#include "kspread_table.h"
#include "kspread_map.h"
#include "kspread_cell.h"
#include "kspread_calc.h"

typedef QValueList<KSpreadValue> valuelist;

// These functions are called from the YACC parser
extern "C" 
{
  extern double cellValue( void* _table, int _x, int _y );
  extern void parseCell( const char *_str, int* _x, int* _y, void **_table );
  extern void parseCell2( const char *_str, int* _x, int* _y, void **_table );
  extern void* parseRange( const char *_str );
  extern void* parseRange2( const char *_str );
  extern void mainParse( const char *_code );
  extern void dependMainParse( const char *_code );
  extern void setResult( double _res );
  extern void setResultBool( char _res );
  extern void setError( int _errno, const char *_txt );
  extern void clearRanges();
  extern void makeCellDepend( const char *_str );
  extern void makeCellDepend2( const char *_str );
  extern void makeRangeDepend( const char *_str );
  extern void makeRangeDepend2( const char *_str );
  extern int funcDbl( const char *_name, void* _args, double* _res );
  extern int funcBool( const char *_name, void* _args, char* _res );
  extern void* newArgList();
  extern void addDbl( void* _args, double _v );
  extern void addBool( void* _args, char _v );
  extern void addRange( void* _args, void* _range );
}

// Used to hold the result of some evaluation
KSpreadValue g_result;

QList<KSpreadRange>* g_lstRanges = 0L;
QList<valuelist>* g_lstArgs = 0L;
QList<KSpreadDepend>* g_pDepends = 0L;
KSpreadTable* g_pTable = 0L;
QString g_errorText;
int g_errno;

/**
 * Called from yacc once the calculation
 * is finished.
 */
void setResult( double _res )
{
  g_result.value.d = _res;
  g_result.type = KSpreadValue::DoubleType;
}

/**
 * Called from yacc once the calculation
 * is finished and if the formula was of
 * boolean type.
 */
void setResultBool( char _res )
{
  g_result.value.b = (bool)_res;
  g_result.type = KSpreadValue::BoolType;
}

/**
 * Creates a new argument list. These lists
 * are used to hold parameters for a function call.
 */
void* newArgList()
{
  valuelist* a = new valuelist;

  // Use the garbage collection
  if ( g_lstArgs == 0 )
  {
    g_lstArgs = new QList<valuelist>;
    g_lstArgs->setAutoDelete( true );
  }
  g_lstArgs->append( a );
  
  return a;
}

/**
 * Add a double value to the argument list.
 */
void addDbl( void* _args, double _v )
{
  valuelist* a = (valuelist*)_args;
  KSpreadValue x;
  x.type = KSpreadValue::DoubleType;
  x.value.d = _v;
  a->append( x );
}

/**
 * Add a boolean value to the argument list.
 */
void addBool( void* _args, char _v )
{
  valuelist* a = (valuelist*)_args;
  KSpreadValue x;
  x.type = KSpreadValue::BoolType;
  x.value.b = (bool)_v;
  a->append( x );
}

/**
 * Add a range to the argument list.
 */
void addRange( void* _args, void* _range )
{
  valuelist* a = (valuelist*)_args;
  KSpreadValue x;
  x.type = KSpreadValue::RangeType;
  x.value.r = (KSpreadRange*)_range;
  a->append( x );
}

/**
 * Dispatches all functions which take one argument of type range.
 *
 * @return -1 if no such function is known. Only builtin functions
 *         are handled in this function.
 */
int rfunc1( const char *_name, KSpreadRange* _arg, double* _res );
/**
 * Dispatches all functions which take one argument of type double.
 *
 * @return -1 if no such function is known. Only builtin functions
 *         are handled in this function.
 */
int func1( const char *_name, double _arg, double* _res );

/**
 * Called from YACC if a function has to be avaulated in a
 * boolean environment.
 */
int funcBool( const char* , void* , char* )
{
  return -1;
}

/**
 * Called from YACC if a function has to be avaulated in a
 * floating point environment.
 */
int funcDbl( const char* _name, void* _args, double* _res )
{
  valuelist* a = (valuelist*)_args;
  int ret = -1;

  // Test for KSpreads hardcoded functions
  if ( a->count() == 1 )
  {   
    if ( (*a)[0].type == KSpreadValue::DoubleType )
      ret = func1( _name, (*a)[0].value.d, _res );
    else if ( (*a)[0].type == KSpreadValue::RangeType )
      ret = rfunc1( _name, (*a)[0].value.r, _res );
  }

  if ( ret != -1 )
    return ret;

  // TODO: Use KScript here!
  /*
  {
    QString cmd = _name;
    cmd += "( ";
    int s = a->count();
    for( int i = 0; i < s; i++ )
    {
      if ( (*a)[i].type == KSpreadValue::DoubleType )
      {  
	char buffer[ 100 ];
	sprintf( buffer, "%f", (*a)[i].value.d );
	cmd += buffer;
      }
      else if ( (*a)[i].type == KSpreadValue::RangeType )
      {  
	char buffer[ 100 ];
	sprintf( buffer, "(%i,%i,%i,%i)", (*a)[i].value.r->range.left(), (*a)[i].value.r->range.top(),
		 (*a)[i].value.r->range.right(), (*a)[i].value.r->range.bottom() );
	cmd += buffer;
      }
      if ( (*a)[i].type == KSpreadValue::BoolType )
      {  
	if ( (*a)[i].value.b )
	  cmd += "1";
	else
	  cmd += "0";
      }
      else
	assert(0);

      if ( i + 1 == s )
	cmd += " )";
      else
	cmd += ", ";
    }
    
    PyObject *obj;
    obj = g_pTable->doc()->pythonModule()->eval( cmd );
    if ( !obj )
    {
      cerr << "ERROR in python stuff" << endl;
      return -3;
    }

    if ( PyArg_Parse( obj, "d", _res ) )
    {
      Py_DECREF( obj );
      return 0;
    }
  
    Py_DECREF( obj );
    
    cerr << "return value is not a double" << endl;
      
  }
  */
  return -1;
}

extern int rfunc1( const char *_name, KSpreadRange* r, double* _res )
{
  if ( strcasecmp( _name, "sum" ) == 0 )
  {
    double res = 0.0;

    for( int x = r->range.left(); x <= r->range.right(); x++ )
      for( int y = r->range.top(); y <= r->range.bottom(); y++ )
      {  
	KSpreadCell *obj = r->table->cellAt( x, y );
	if ( obj->isValue() )
	  res += obj->valueDouble();
	/* else
	{    
	  setError( ERR_NOT_A_NUMERIC_VALUE, "" );
	  *_res = 1.0;
	  return -1;
	  } */
      }
    
    *_res = res;
    return 0;
  }

  return -1;
}

extern int func1( const char *_name, double _arg, double* _res )
{
  if ( strcasecmp( _name, "sqrt" ) == 0 )
  {
    *_res = sqrt( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "exp" ) == 0 )
  {
    *_res = exp( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "ln" ) == 0 )
  {
    *_res = log( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "log" ) == 0 )
  {
    *_res = log10( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "sin" ) == 0 )
  {
    *_res = sin( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "cos" ) == 0 )
  {
    *_res = cos( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "tan" ) == 0 )
  {
    *_res = tan( _arg );
    return 0;
  }
  if ( strcasecmp( _name, "asin" ) == 0 )
  {
    *_res = sin( _arg );
    if ( errno == EDOM )
      return -2;
    return 0;
  }
  if ( strcasecmp( _name, "acos" ) == 0 )
  {
    *_res = cos( _arg );
    if ( errno == EDOM )
      return -2;
    return 0;
  }
  if ( strcasecmp( _name, "atan" ) == 0 )
  {
    *_res = atan( _arg );
    if ( errno == EDOM )
      return -2;
    return 0;
  }
      
  return -1;
}

void setError( int _errno, const char *_txt )
{
  g_errno = _errno;
  g_errorText = _txt;
  g_result.type = KSpreadValue::ErrorType;
  
  cerr << "MyError "  << _errno << " " << _txt << endl;
}

void clearParser()
{
  if ( g_lstRanges )
    g_lstRanges->clear();
  if ( g_lstArgs )
    g_lstArgs->clear();
}

KSpreadTable* findTable( const char *_name )
{
  cerr << "Searching table " << _name << endl;

  KSpreadTable *t = g_pTable->map()->findTable( _name );
  if ( t != 0L )
    return t;
  
  setError( ERR_UNKNOWN_TABLE, _name );
  return 0L;
}

void parseCell( const char *_str, int* _x, int* _y, void** _table )
{
  KSpreadPoint p( _str );

  ASSERT( p.isValid() );
  
  *_x = p.pos.x();
  *_y = p.pos.y();
  *_table = g_pTable;
}

void parseCell2( const char *_str, int* _x, int* _y, void** _table )
{
  KSpreadPoint p( _str, g_pTable->map() );

  ASSERT( p.isValid() && p.isTableKnown() );
  
  *_x = p.pos.x();
  *_y = p.pos.y();
  *_table = p.table;
}

void* parseRange( const char *_str )
{
  return new KSpreadRange( _str );
}

void* parseRange2( const char *_str )
{
  return new KSpreadRange( _str, g_pTable->map() );
}

double cellValue( void *_table, int _x, int _y )
{
  if ( _table == 0L )
  {
    debug("cellValue: Dont have table");
    return 1.0;
  }
  
  KSpreadTable *t = (KSpreadTable*)_table;
  KSpreadCell *obj = t->cellAt( _x, _y );

  if ( obj->isValue() )
    return obj->valueDouble();

  // setError( ERR_NOT_A_NUMERIC_VALUE, "" );
  
  cerr << "No value at " << t->name().ascii() << " (" << _x << "|" << _y << ")" << endl;
  
  return 0.0;
}

void makeCellDepend( const char *_str )
{
  int x,y;
  void *table;
  parseCell( _str, &x, &y, &table );

  if ( g_errno != 0 )
    return;
  
  KSpreadDepend* dep = new KSpreadDepend;
  
  dep->m_iColumn = x;
  dep->m_iRow = y;
  dep->m_iColumn2 = -1;
  dep->m_iRow2 = -1;
  dep->m_pTable = g_pTable;
  g_pDepends->append( dep );
  
  cerr << "Depends on " << x << "/" << y << endl;
}

void makeCellDepend2( const char *_str )
{
  int x,y;
  void *table;
  parseCell2( _str, &x, &y, &table );

  if ( g_errno != 0 )
    return;
  
  KSpreadDepend* dep = new KSpreadDepend;
  
  dep->m_iColumn = x;
  dep->m_iRow = y;
  dep->m_iColumn2 = -1;
  dep->m_iRow2 = -1;
  dep->m_pTable = (KSpreadTable*)table;
  g_pDepends->append( dep );

  cerr << "Depends on " << x << "/" << y << endl;
}

void makeRangeDepend( const char *_str )
{
  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, ':' );
  *p2++ = 0;

  int x1,y1,x2,y2;
  void* dummy;
  parseCell( p, &x1, &y1, &dummy );
  parseCell( p2, &x2, &y2, &dummy );

  if ( g_errno != 0 )
    return;
  
  KSpreadDepend *dep = new KSpreadDepend;
  dep->m_iColumn = x1;
  dep->m_iRow = y1;
  dep->m_iColumn2 = x2;
  dep->m_iRow2 = y2;
  dep->m_pTable = g_pTable;
  g_pDepends->append( dep );

  cerr << "Depends on " << x1 << "/" << y1 << " " << x2 << "/" << y2 << endl;
}

void makeRangeDepend2( const char *_str )
{
  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, '!' );
  *p2++ = 0;
  char *p3 = strchr( p2, ':' );
  *p3++ = 0;

  int x1,y1,x2,y2;
  void* dummy;
  parseCell( p2, &x1, &y1, &dummy );
  parseCell( p3, &x2, &y2, &dummy );

  KSpreadTable* t = findTable( p );
  
  if ( g_errno != 0 )
    return;
  
  KSpreadDepend *dep = new KSpreadDepend;
  dep->m_iColumn = x1;
  dep->m_iRow = y1;
  if ( x1 == x2 && y1 == y1 )
  {
    dep->m_iColumn2 = -1;
    dep->m_iRow2 = -1;
  }
  else
  {
    dep->m_iColumn2 = x2;
    dep->m_iRow2 = y2;
  }
  dep->m_pTable = t;
  g_pDepends->append( dep );

  cerr << "Depends on " << p << " " << x1 << "/" << y1 << " " << x2 << "/" << y2 << endl;
}

KSpreadValue evalFormular( const char *_formular, KSpreadTable* _table )
{
  cerr << "Scanning" << _formular << endl;
  
  g_pTable = _table;
  
  g_errno = 0;
  mainParse( _formular );
  clearParser();

  return g_result;
}

bool makeDepend( const char* _formular, KSpreadTable* _table, QList<KSpreadDepend>* _list )
{
  g_pTable = _table;
  g_pDepends = _list;
  g_pDepends->clear();
  
  g_errno = 0;
  dependMainParse( _formular );

  clearParser();

  if ( g_errno != 0 )
    return false;

  return true;
}
  
/*
int main( int argc, char **argv )
{
  dependMainParse( argv[1] );
  cerr << "----------------------" << endl;
  
  mainParse( argv[ 1 ] );
  clearRanges();
  
  cerr << "The result is " << result << endl;
}
*/
