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

#include "komlFileFeed.h"

KOMLFileFeed::KOMLFileFeed( const char* _filename )
{
  m_fh = fopen( _filename, "rb" );
}

KOMLFileFeed::~KOMLFileFeed()
{
  if ( m_fh )
    fclose( m_fh );
}

KOMLData* KOMLFileFeed::read()
{
  if ( !m_fh )
    return 0L;
  
  char* buffer = new char[ 11 ];
  size_t res = fread( buffer, 1, 10, m_fh );
  if ( res <= 0 )
  {
    fclose( m_fh );
    m_fh = 0L;
    return 0L;
  }
  
  buffer[ res ] = 0;
  return new KOMLData( buffer, res );
}

void KOMLFileFeed::free( KOMLData* _data )
{
  if ( _data )
    delete _data;
}
