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

#include "torben.h"

#include <ctype.h>

using namespace std;

string& tstring::stripWhiteSpace( string& _res ) const
{
  int len = length();
  int i = 0;
  while( i < len && isspace( data()[i] ) )
    i++;

  if ( i == len )
  {
    _res = "";
    return _res;
  }

  int j = len - 1;
  while( j >= 0 && isspace( data()[j] ) )
    j--;

  _res.assign( c_str() + i, j - i + 1 );
  return _res;
}

string& tstring::stripWhiteSpace()
{
  int len = length();
  int i = 0;
  while( i < len && isspace( data()[i] ) )
    i++;

  if ( i == len )
  {
    *this = "";
    return *this;
  }

  erase( 0, i );

  len = length();
  int j = len - 1;
  while( j >= 0 && isspace( data()[j] ) )
    j--;

  if ( j == len - 1 )
    return (*this);

  erase( j + 1, len - j - 1 );
  return (*this);
}

string& tstring::right( string &_res, int n ) const
{
  int len = length();
  if ( len >= n )
  {
    _res = *this;
    return _res;
  }

  _res.assign( c_str() + len - n );
  return _res;
}
