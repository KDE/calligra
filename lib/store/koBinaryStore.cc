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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <kdebug.h>

#include "koBinaryStore.h"

KoBinaryStore::KoBinaryStore( const char* _filename, KOStore::Mode _mode )
{
  m_bIsOpen = false;
  m_mode = _mode;
  m_id = 0;
  
  kdebug( KDEBUG_INFO, 30002, "KoBinaryStore Constructor filename = %s mode = %d", _filename, _mode);

  if ( _mode == KOStore::Write )
  {
    m_out.open( _filename, ios::out | ios::trunc );
    m_out.write( "KS01", 4 );
  }
  else if ( _mode == KOStore::Read )
  {
    m_in.open( _filename, ios::in );
    // Skip header
    m_in.seekg( 4 );

    while( !m_in.eof() )
    {
      Entry e;
      if ( readHeader( e ) )
      {  
	m_in.seekg( e.size, ios::cur );
	m_map[ e.name ] = e;
      }
    }
  }
  else
    assert( 0 );
}

KoBinaryStore::~KoBinaryStore()
{
  kdebug( KDEBUG_INFO, 30002, "###################### DESTRUCTOR ####################" );
  if ( m_mode == KOStore::Write )
  {
    m_out.close();
  }
  else
    m_in.close();
}

void KoBinaryStore::writeHeader( const KoBinaryStore::Entry& _entry )
{
  int len = _entry.name.size() + 1 + _entry.mimetype.size() + 1 + 4 + 4 + 4 + 4 + 4;
  putULong( len );
  putULong( _entry.size );
  putULong( 0 ); // used to be "flags", unused.
  putULong( _entry.mimetype.size() );
  m_out.write( _entry.mimetype.c_str(), _entry.mimetype.size() + 1 );
  putULong( _entry.name.size() );
  m_out.write( _entry.name.c_str(), _entry.name.size() + 1 );
}

unsigned long KoBinaryStore::readHeader( KoBinaryStore::Entry& _entry )
{
  _entry.pos = m_in.tellg();
  unsigned long len = getULong();
  if ( m_in.eof() )
    return 0L;
  
  _entry.size = getULong();
  /*unsigned long flags = */ getULong();

  unsigned int s = getULong();
  char *str = new char[ s + 1 ];
  m_in.read( str, s + 1 );
  _entry.mimetype = str;
  delete []str;

  s = getULong();
  str = new char[ s + 1 ];
  m_in.read( str, s + 1 );
  _entry.name = str;
  delete []str;
  _entry.data = m_in.tellg();

  return len;
}
  
void KoBinaryStore::putULong( unsigned long x )
{
  int n;
  for ( n = 0; n < 4; n++ )
  {
    m_out.put( (char)(x & 0xff) );
    x >>= 8;
  }
}

unsigned long KoBinaryStore::getULong()
{
  unsigned long x = m_in.get();
  x += ( (unsigned long)m_in.get() ) << 8;
  x += ( (unsigned long)m_in.get() ) << 16;
  x += ( (unsigned long)m_in.get() ) << 24;
  
  return x;
}

/*
char* KoBinaryStore::createFileName()
{
  char buffer[ 100 ];
  sprintf( buffer, "entry%i", m_id++ );
  return CORBA::string_dup( buffer );
}
*/

void KoBinaryStore::list()
{
  cout << "Size\tType\t\tName" << endl;
  cout << "--------------------------------------------------------------------" << endl;

  unsigned int size = 0;
  
  map<string,Entry>::iterator it = m_map.begin();
  for( ; it != m_map.end(); ++it )
  {
    size += it->second.size;
    cout << it->second.size << "\t" << it->second.mimetype << "\t" << it->second.name << endl;
  }

  cout << "--------------------------------------------------------------------" << endl;
  cout << "Total Size: " << size << endl;
}

CORBA::Boolean KoBinaryStore::open( const char* _name, const char *_mime_type )
{
  kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: opening for %s, mimetype %s", _name, _mime_type);
  if ( m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: File is already opened" );
    return false;
  }
    
  if ( !_mime_type && m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Mimetype omitted while opening entry %c for writing", _name );
    return false;
  }
  
  if ( strlen( _name ) > 512 )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Filename %c is too long", _name );
    return false;
  }
  
  if ( m_mode == KOStore::Write )
  {
    if ( m_map.find( _name ) != m_map.end() )
    {
      kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Duplicate filename %c", _name );
      return false;
    }
    
    m_current.pos = m_out.tellp();
    m_current.name = _name;
    m_current.mimetype = _mime_type;
    m_current.size = 0;
    // We will write this header again later once we know the size
    writeHeader( m_current );
    m_current.data = m_out.tellp();
  }
  else if ( m_mode == KOStore::Read )
  { 
    kdebug( KDEBUG_INFO, 30002, "Opening for reading %c", _name );
    
    map<string,Entry>::iterator it = m_map.find( _name );
    if ( it == m_map.end() )
    {
      kdebug( KDEBUG_INFO, 30002, "Unknown filename %c", _name );
      return false;
    }
    if ( _mime_type && strlen( _mime_type ) != 0 && it->second.mimetype != _mime_type )
    {
      kdebug( KDEBUG_INFO, 30002, "Wrong mime_type in file %c", _name );
      kdebug( KDEBUG_INFO, 30002, "Expected %c but got %c", _mime_type, it->second.mimetype.c_str() );
      return false;
    }
    m_in.seekg( it->second.data );
    m_readBytes = 0;
    m_current = it->second;
    m_in.clear();
  }
  else
    assert( 0 );
  
  m_bIsOpen = true;

  return true;
}

void KoBinaryStore::close()
{
  kdebug( KDEBUG_INFO, 30002, "koBinaryStore: Closing" );
  
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: You must open before closing" );
    return;
  }

  if ( m_mode == KOStore::Write )
  {
    // rewrite header with correct size
    m_current.size = m_out.tellp() - m_current.data;
    m_out.seekp( m_current.pos );
    writeHeader( m_current );
    m_out.seekp( 0, ios::end );
    // missing : m_map.insert( name, m_current ) ....
  }

  m_bIsOpen = false;
}

KOStore::Data* KoBinaryStore::read( CORBA::ULong max )
{
  KOStore::Data* data = new KOStore::Data;

  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: You must open before reading" );
    data->length( 0 );
    return data;
  }
  if ( m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Can not read from store that is opened for writing" );
    data->length( 0 );
    return data;
  }
  
  if ( m_in.eof() )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF" );
    data->length( 0 );
    return data;
  }
  
  if ( max > m_current.size - m_readBytes )
    max = m_current.size - m_readBytes;
  if ( max == 0 )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF 2" );
    data->length( 0 );
    return data;
  }
  
  unsigned char *p = new unsigned char[ max ];
  m_in.read( p, max );
  unsigned int len = m_in.gcount();
  if ( len != max )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Error while reading" );
    data->length( 0 );
    return data;
  }
  
  m_readBytes += max;
  data->length( max );
  for( unsigned int i = 0; i < max; i++ )
    (*data)[i] = p[i];
  delete [] p;

  return data;
}

long KoBinaryStore::read( char *_buffer, unsigned long _len )
{
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: You must open before reading" );
    return -1;
  }
  if ( m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Can not read from store that is opened for writing" );
    return -1;
  }
  
  if ( m_in.eof() )
    return 0;
  
  if ( _len > m_current.size - m_readBytes )
    _len = m_current.size - m_readBytes;
  if ( _len == 0 )
    return 0;
  
  m_in.read( _buffer, _len );
  unsigned int len = m_in.gcount();
  if ( len != _len )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Error while reading" );
    return -1;
  }
  
  m_readBytes += len;
  
  return len;
}

CORBA::Boolean KoBinaryStore::write( const KOStore::Data& data )
{
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: You must open before writing" );
    return 0L;
  }
  if ( m_mode != KOStore::Write )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Can not write to store that is opened for reading" );
    return 0L;
  }

  unsigned int len = data.length();
  unsigned char *p = new unsigned char[ len ];
  for( unsigned int i = 0; i < len; i++ )
    p[i] = data[i];
    
  m_out.write( p, len );
  m_current.size += len;
  
  delete [] p;

  if ( bad() )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Error while writing" );
    return false;
  }
  return true;
}

bool KoBinaryStore::write( const char* _data, unsigned long _len )
{
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: You must open before writing" );
    return 0L;
  }
  if ( m_mode != KOStore::Write )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Can not write to store that is opened for reading" );
    return 0L;
  }

  m_out.write( _data, _len );
  m_current.size += _len;

  if ( bad() )
  {
    kdebug( KDEBUG_INFO, 30002, "KoBinaryStore: Error while writing" );
    return false;
  }

  return true;
}
