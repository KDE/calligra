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

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string>

#include <kapp.h>

QString util_columnLabel( int column )
{
  char buffer[ 100 ];

  if ( column <= 26 )
    sprintf( buffer, "%c", 'A' + column - 1 );
  else if ( column <= 26 * 26 )
    sprintf( buffer, "%c%c",'A'+((column-1)/26)-1,'A'+((column-1)%26));
  else
    sprintf( buffer, "%c%c",'Y','Z');
    //sprintf( buffer, "%c%c",'A'+((26-1)/26)-1,'A'+((26-1)%26));
    //limit is 26*26
    //strcpy( buffer,"@@@");

  return QString( buffer );
}

QString util_cellName( int _col, int _row )
{
  QString result( "%1%2" );
  result = result.arg( util_columnLabel( _col ) ).arg( _row );
  return result;
}

QString util_cellName( KSpreadTable* table, int _col, int _row )
{
  QString result( "%1!%2%3" );
  result = result.arg( table->tableName() ).arg( util_columnLabel( _col ) ).arg( _row );
  return result;
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
  QString result( _table->tableName() );
  result += "!";
  result += util_rangeName( _area );

  return result;
}

KSpreadPoint::KSpreadPoint( const QString& _str )
{
  table = 0;
  init( _str );
}

void KSpreadPoint::init( const QString& _str )
{
  pos.setX( -1 );

  if ( _str.isEmpty() )
    return;

  uint p = 0;
  uint len = _str.length();

  // Fixed ?
  if ( _str[p] == '$' )
  {
    columnFixed = true;
    p++;
  } else
    columnFixed = false;

  // Malformed ?
  /*if ( p == len || _str[p] < 'A' || _str[p] > 'Z' )
    return;
  */
  if ( p == len || _str[p] < 'A' || _str[p] > 'Z' )
        {
        if(_str[p] < 'a' || _str[p] > 'z' )
                return;
        }
  int offset='a'-'A';
  int x=-1;
  if(_str[p] >= 'A' && _str[p] <= 'Z')
         x= _str[p++] - 'A' + 1;
  else if(_str[p] >= 'a' && _str[p] <= 'z')
        x= _str[p++]-offset - 'A' + 1;

  // Malformed ?
  if ( p == len  )
    return;

  while( p < len && ((_str[p] >= 'A' && _str[p] <= 'Z')||(_str[p] >= 'a' && _str[p] <= 'z' )))
    {
    if(_str[p] >= 'A' && _str[p] <= 'Z')
        x = x*26 + (char)_str[p++] - 'A' + 1;
    else if(_str[p] >= 'a' && _str[p] <= 'z')
        x = x*26 + (char)_str[p++] - 'A' + 1-offset;
    }

  //limit is 26*26
  if(x>(26*26))
    return;
  // Malformed ?
  if ( p == len  )
    return;

  if ( _str[p] == '$' )
  {
    rowFixed = true;
    p++;
    // Malformed ?
    if ( p == len  )
      return;
  }
  else
    rowFixed = false;

  uint p2 = p;
  while( p < len )
  {
    if ( !isdigit( _str[p++] ) )
      return;
  }

  int y = atoi( _str.latin1() + p2 );
  if ( y <= 0 )
    return;

  pos = QPoint( x, y );
}

KSpreadPoint::KSpreadPoint( const QString& _str, KSpreadMap* _map, KSpreadTable* _table )
{
  uint p = 0;
  int p2 = _str.find( "!" );
  if ( p2 != -1 )
  {
    tableName = _str.left( p2++ );
    table = _map->findTable( tableName );
    p = p2;
  }
  else
    table = _table;

  init( _str.mid( p ) );
}

KSpreadCell* KSpreadPoint::cell()
{
  return table->cellAt( pos );
}

KSpreadRange::KSpreadRange( const QString& _str )
{
  range.setLeft( -1 );
  table = 0;

  int p = _str.find( ":" );
  if ( p == -1 )
    return;

  KSpreadPoint ul( _str.left( p ) );
  KSpreadPoint lr( _str.mid( p + 1 ) );
  range = QRect( ul.pos, lr.pos );

  leftFixed = ul.columnFixed;
  rightFixed = lr.columnFixed;
  topFixed = ul.rowFixed;
  bottomFixed = lr.rowFixed;
}

KSpreadRange::KSpreadRange( const QString& _str, KSpreadMap* _map, KSpreadTable* _table )
{
  range.setLeft( -1 );
  table = 0;

  int p = 0;
  int p2 = _str.find( "!" );
  if ( p2 != -1 )
  {
    tableName = _str.left( p2++ );
    table = _map->findTable( tableName );
    p = p2;
  }
  else
    table = _table;

  int p3 = _str.find( ":", p );
  if ( p3 == -1 )
    return;

  KSpreadPoint ul( _str.mid( p, p3 - p ) );
  KSpreadPoint lr( _str.mid( p3 + 1 ) );
  range = QRect( ul.pos, lr.pos );

  leftFixed = ul.columnFixed;
  rightFixed = lr.columnFixed;
  topFixed = ul.rowFixed;
  bottomFixed = lr.rowFixed;
}
