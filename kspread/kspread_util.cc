#include "kspread_util.h"
#include "kspread_map.h"

KSpread::Cell util_parseCell( const char *_str )
{
  KSpread::Cell c;

  char *p = new char[ strlen( _str ) + 1 ];
  char *orig = p;
  strcpy( p, _str );

  if ( *p == '$' )
    p++;

  if ( *p < 'A' || *p > 'Z' )
  {
    KSpread::MalformedExpression exc;
    exc.expr = CORBA::string_dup( _str );
    mico_throw( exc );
  }
  
  int x = *p++ - 'A';
  int y = 0;
  while( *p >= 'A' && *p <= 'Z' )
    x = (x+1)*26 + *p++ - 'A';

  if ( *p == '$' )
    p++;

  char *p3 = p;
  while( *p3 )
  {
    if ( !isdigit( *p3++ ) )
    {
      KSpread::MalformedExpression exc;
      exc.expr = CORBA::string_dup( _str );
      mico_throw( exc );
    }
  }
  
  y = atoi( p );
  
  delete[] orig;

  c.x = x + 1;
  c.y = y;
  
  return c;  
}

KSpread::Cell util_parseCell( const char *_str, KSpreadMap* _map )
{
  KSpread::Cell c;
  
  char *orig = new char[ strlen( _str ) + 1 ];
  strcpy( orig, _str );
  char *p = orig;
  char *p2 = strchr( p, '!' );
  if ( p2 )
  {    
    
    *p2++ = 0;
  
    KSpreadTable* table = _map->findTable( p );
    if ( !table )
    {
      KSpread::UnknownTable exc;
      exc.table = CORBA::string_dup( p );
      delete []orig;
      mico_throw( exc );
    }

    c.table = CORBA::string_dup( table->name() );
    p = p2;
  }
  else
    c.table = CORBA::string_dup( "" );

  c = util_parseCell( p );

  delete []orig;
  
  return c;
}

KSpread::Range util_parseRange( const char *_str )
{
  KSpread::Range r;
  
  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, ':' );
  if ( !p2 )
  {
    KSpread::MalformedExpression exc;
    exc.expr = CORBA::string_dup( _str );
    mico_throw( exc );
  }
  *p2++ = 0;

  KSpread::Cell c1 = util_parseCell( p );
  KSpread::Cell c2 = util_parseCell( p2 );
  
  delete []p;

  r.left = c1.x;
  r.top = c1.y;
  r.right = c2.x;
  r.bottom = c2.y;
  
  return r;
}

KSpread::Range util_parseRange2( const char *_str, KSpreadMap* _map )
{
  KSpread::Range r;

  char *p = new char[ strlen( _str ) + 1 ];
  strcpy( p, _str );
  char *p2 = strchr( p, '!' );

  if ( p2 )
  {    
    *p2++ = 0;

    KSpreadTable* table = _map->findTable( p );
    if ( !table )
    {
      KSpread::UnknownTable exc;
      exc.table = CORBA::string_dup( p );
      delete []p;
      mico_throw( exc );
    }
  }
  else
  {
    p2 = p;
    r.table = CORBA::string_dup( "" );
  }
  
  char *p3 = strchr( p2, ':' );
  *p3++ = 0;

  KSpread::Cell c1 = util_parseCell( p );
  KSpread::Cell c2 = util_parseCell( p2 );

  delete []p;

  r.left = c1.x;
  r.top = c1.y;
  r.right = c2.x;
  r.bottom = c2.y;

  return r;
}
