// We have to include this because Qt is shit
#include <qprinter.h>
#include "kspread_doc.h"

#include <iostream>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <qlist.h>
#include <qstring.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>

#include "kspread_table.h"
#include "kspread_map.h"
#include "kspread_cell.h"
#include "kspread_calc.h"

struct range
{
  KSpreadTable *table;
  int fromX;
  int fromY;
  int toX;
  int toY;
};

enum MyArgType { AT_Double, AT_Range };
struct myarg
{
  MyArgType type;
  range* r;
  double d;
};

typedef vector<myarg> myarglist;

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
  extern void setError( int _errno, const char *_txt );
  extern void clearRanges();
  extern void makeCellDepend( const char *_str );
  extern void makeCellDepend2( const char *_str );
  extern void makeRangeDepend( const char *_str );
  extern void makeRangeDepend2( const char *_str );
  extern int funcDbl( const char *_name, void* _args, double* _res );
  extern void* newArgList();
  extern void addDbl( void* _args, double _v );
  extern void addRange( void* _args, void* _range );
}

double result;
QList<range>* g_lstRanges = 0L;
QList<myarglist>* g_lstArgs = 0L;
QList<KSpreadDepend>* g_pDepends = 0L;
KSpreadTable* g_pTable = 0L;
QString g_errorText;
int g_errno;

void setResult( double _res )
{
  result = _res;
}

void* newArgList()
{
  myarglist* a = new myarglist;

  if ( g_lstArgs == 0 )
  {
    g_lstArgs = new QList<myarglist>;
    g_lstArgs->setAutoDelete( true );
  }
  g_lstArgs->append( a );
  
  return a;
}

void addDbl( void* _args, double _v )
{
  myarglist* a = (myarglist*)_args;
  myarg x;
  x.type = AT_Double;
  x.d = _v;
  a->push_back( x );
}

void addRange( void* _args, void* _range )
{
  myarglist* a = (myarglist*)_args;
  myarg x;
  x.type = AT_Range;
  x.r = (range*)_range;
  a->push_back( x );
}

int rfunc1( const char *_name, range* _arg, double* _res );
int func1( const char *_name, double _arg, double* _res );

int funcDbl( const char* _name, void* _args, double* _res )
{
  myarglist* a = (myarglist*)_args;
  int ret = -1;

  if ( a->size() == 1 )
  {   
    if ( (*a)[0].type == AT_Double )
      ret = func1( _name, (*a)[0].d, _res );
    else if ( (*a)[0].type == AT_Range )
      ret = rfunc1( _name, (*a)[0].r, _res );
  }

  if ( ret != -1 )
    return ret;

  // Python  
  {
    QString cmd = _name;
    cmd += "( ";
    int s = a->size();
    for( int i = 0; i < s; i++ )
    {
      if ( (*a)[i].type == AT_Double )
      {  
	char buffer[ 100 ];
	sprintf( buffer, "%f", (*a)[i].d );
	cmd += buffer;
      }
      else if ( (*a)[i].type == AT_Range )
      {  
	char buffer[ 100 ];
	sprintf( buffer, "(%i,%i,%i,%i)", (*a)[i].r->fromX, (*a)[i].r->fromY, (*a)[i].r->toX, (*a)[i].r->toY );
	cmd += buffer;
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
  
    /* char *str;
       if ( !PyArg_Parse( v, "s", &str ) )
       {
       printf(" Could not parse\n");
       return FALSE;
       }
       _result = (const char*)str; */
    
  }
  
  return -1;
}

extern int rfunc1( const char *_name, range* r, double* _res )
{
  if ( strcasecmp( _name, "sum" ) == 0 )
  {
    double res = 0.0;
    cerr << "Calculating range from " << r->fromX << "/" << r->fromY << " " << r->toX << "/" << r->toY << endl;
    for( int x = r->fromX; x <= r->toX; x++ )
      for( int y = r->fromY; y <= r->toY; y++ )
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
  if ( *_str == '$' )
    _str++;
  
  int x = *_str++ - 'A';
  int y = 0;
  while( *_str >= 'A' && *_str <= 'Z' )
    x = (x+1)*26 + *_str++ - 'A';

  if ( *_str == '$' )
    _str++;

  y = atoi( _str );
  
  *_x = x + 1;
  *_y = y;
  *_table = g_pTable;
}

void parseCell2( const char *_str, int* _x, int* _y, void** _table )
{
  char *p = new char[ strlen( _str ) + 1 ];
  char *orig = p;
  strcpy( p, _str );
  char *p2 = strchr( p, '!' );
  *p2++ = 0;

  *_table = findTable( p );
  // TODO: check wether this table really exists

  if ( *p == '$' )
    p++;
  
  int x = *p++ - 'A';
  int y = 0;
  while( *p >= 'A' && *p <= 'Z' )
    x = (x+1)*26 + *p++ - 'A';

  if ( *p == '$' )
    p++;

  y = atoi( p );
  
  delete[] orig;

  *_x = x + 1;
  *_y = y;
}

void* parseRange( const char *_str )
{
  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, ':' );
  *p2++ = 0;

  struct range* r = new range;

  void* dummy;
  parseCell( p, &(r->fromX), &(r->fromY), &dummy );
  parseCell( p2, &(r->toX), &(r->toY), &dummy );
  r->table = g_pTable;
  
  delete []p;

  if ( g_lstRanges == 0L )
  {       
    g_lstRanges = new QList<range>;
    g_lstRanges->setAutoDelete( true );
  }
  
  g_lstRanges->append( r );
  return r;
}

void* parseRange2( const char *_str )
{
  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, '!' );
  *p2++ = 0;
  char *p3 = strchr( p2, ':' );
  *p3++ = 0;

  struct range* r = new range;

  void* dummy;
  parseCell( p2, &(r->fromX), &(r->fromY), &dummy );
  parseCell( p3, &(r->toX), &(r->toY), &dummy );

  if ( g_lstRanges == 0L )
  {       
    g_lstRanges = new QList<range>;
    g_lstRanges->setAutoDelete( true );
  }

  r->table = findTable( p );
  delete []p;

  // Did an error occur during parsing ?
  if ( g_errno != 0L )
  {
    delete r;
    return 0L;
  }
  
  g_lstRanges->append( r );

  return r;
}

double cellValue( void *_table, int _x, int _y )
{
  if ( _table == 0L )
    return 1.0;

  KSpreadTable *t = (KSpreadTable*)_table;
  KSpreadCell *obj = t->cellAt( _x, _y );

  if ( obj->isValue() )
    return obj->valueDouble();

  // setError( ERR_NOT_A_NUMERIC_VALUE, "" );
  
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

bool evalFormular( const char *_formular, KSpreadTable* _table, double& _result )
{
  cerr << "Scanning" << _formular << endl;
  
  g_pTable = _table;
  
  g_errno = 0;
  mainParse( _formular );
  clearParser();

  if ( g_errno != 0 )
    return false;

  _result = result;
  return true;
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
