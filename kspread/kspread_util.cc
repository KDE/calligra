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

#include "kspread_util.h"
#include "kspread_map.h"
#include "kspread_table.h"

QString util_columnLabel( int column )
{
  char buffer[ 100 ];
  
  if ( column <= 26 )
    sprintf( buffer, "%c", 'A' + column - 1 );
  else if ( column <= 26 * 26 )
    sprintf( buffer, "%c%c",'A'+((column-1)/26)-1,'A'+((column-1)%26));
  else
    strcpy( buffer,"@@@");

  return QString( buffer );
}                           

QString util_cellName( int _col, int _row )
{
  QString label( util_columnLabel( _col ) );
  
  char buffer[ 20 ];
  sprintf( buffer, "%s%d", label.data(), _row );

  return QString( buffer );
}

QString util_rangeName( QRect _area )
{
  QString result;
  result = util_cellName( _area.left(), _area.top() );
  result += ":";
  result += util_cellName( _area.right(), _area.bottom() );
  
  return result;
}

QString util_rangeName( KSpreadTable *_table, QRect _area )
{
  QString result( _table->name() );
  result += "!";
  result += util_rangeName( _area );
  
  return result;
}

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
    r.table = CORBA::string_dup( p );
  }
  else
  {
    p2 = p;
    r.table = CORBA::string_dup( "" );
  }
  
  char *p3 = strchr( p2, ':' );
  if ( !p3 )
  {
    KSpread::MalformedExpression exc;
    exc.expr = CORBA::string_dup( _str );
    mico_throw( exc );
  }
  *p3++ = 0;

  KSpread::Cell c1 = util_parseCell( p2 );
  KSpread::Cell c2 = util_parseCell( p3 );

  delete []p;

  r.left = c1.x;
  r.top = c1.y;
  r.right = c2.x;
  r.bottom = c2.y;

  return r;
}

