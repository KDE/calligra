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

#include "koTarStore.h"

#include <kdebug.h>
#include <ktar.h>

KoTarStore::KoTarStore( const char* _filename, KOStore::Mode _mode )
{
  m_bIsOpen = false;
  m_mode = _mode;
  m_id = 0;
  
  kdebug( KDEBUG_INFO, 30002, "KoTarStore Constructor filename = %s mode = %d", _filename, _mode);

  m_pTar = new KTar( _filename );

  m_bGood = m_pTar->open( _mode == KOStore::Write ? IO_WriteOnly : IO_ReadOnly );
}

KoTarStore::~KoTarStore()
{
  kdebug( KDEBUG_INFO, 30002, "###################### DESTRUCTOR ####################" );
  m_pTar->close();
  delete m_pTar;
}
/*
char* KoTarStore::createFileName()
{
  char buffer[ 100 ];
  sprintf( buffer, "entry%i", m_id++ );
  return CORBA::string_dup( buffer );
}
*/

QString KoTarStore::toExternalNaming( const QString _internalNaming )
{
  // "root" is the main document, let's save it as "maindoc.xml"
  if (_internalNaming == "root") 
    return "maindoc.xml";

  // store:/0 is saved as part0.xml
  if (_internalNaming.left(7) == "store:/")
    return QString("part") + _internalNaming.mid(7) + ".xml";

  // it seems we have a problem when length() > 100...
  // No idea why (filenames get truncated in the tar.gz file)
  // Possible hack for kpresenter pictures to do here.
  //

  return _internalNaming;
}

CORBA::Boolean KoTarStore::open( const char* _name, const char * /*_mime_type*/ )
{
  kdebug( KDEBUG_INFO, 30002, "KoTarStore: opening for '%s'", _name);
  if ( m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: File is already opened" );
    return false;
  }
    
  if ( strlen( _name ) > 512 )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Filename %s is too long", _name );
    return false;
  }
  
  if ( m_mode == KOStore::Write )
  {
    if ( m_strFiles.findIndex( _name ) != -1 ) // just check if it's there
    {
      kdebug( KDEBUG_INFO, 30002, "KoTarStore: Duplicate filename %s", _name );
      return false;
    }
    
    m_strFiles.append( _name );
    m_sName = _name;
    m_iSize = 0;
  }
  else if ( m_mode == KOStore::Read )
  { 
    m_sName = toExternalNaming( _name );

    kdebug( KDEBUG_INFO, 30002, "Opening for reading '%s'", m_sName.latin1() );
    
    const KTarEntry * entry = m_pTar->directory()->entry( m_sName );
    if ( entry == 0L )
    {
      kdebug( KDEBUG_INFO, 30002, "Unknown filename '%s'", m_sName.latin1() );
      return false;
    }
    if ( entry->isDirectory() )
    {
      kdebug( KDEBUG_INFO, 30002, "'%s' is a directory !", m_sName.latin1() );
      return false;
    }
    KTarFile * f = (KTarFile *) entry;
    m_byteArray = f->data();
    m_iSize = f->data().size(); 
    // it seems m_byteArray might be bigger than f->data().size() (if a previous file was bigger)
    m_readBytes = 0;
  }
  else
    assert( 0 );
  
  m_stream = new QDataStream( m_byteArray, (m_mode == KOStore::Write) ? IO_WriteOnly : IO_ReadOnly);
  m_bIsOpen = true;

  return true;
}

void KoTarStore::close()
{
  kdebug( KDEBUG_INFO, 30002, "koTarStore: Closing" );
  
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: You must open before closing" );
    return;
  }

  if ( m_mode == KOStore::Write )
  {
    // write the whole bytearray at once into the tar file
  
    m_sName = toExternalNaming( m_sName );

    kdebug( KDEBUG_INFO, 30002, "Writing file %s into TAR archive. size %d. [byteArray size %d]",
          m_sName.latin1(), m_iSize, m_byteArray.size() );
    m_pTar->writeFile( m_sName , "user", "group", m_iSize, m_byteArray.data() );
  }

  delete m_stream;
  m_stream = 0L;
  m_bIsOpen = false;
}

KOStore::Data* KoTarStore::read( CORBA::ULong max )
{
  KOStore::Data* data = new KOStore::Data;

  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: You must open before reading" );
    data->length( 0 );
    return data;
  }
  if ( m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Can not read from store that is opened for writing" );
    data->length( 0 );
    return data;
  }
  
  if ( m_stream->atEnd() )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF" );
    data->length( 0 );
    return data;
  }
  
  if ( max > m_iSize - m_readBytes )
    max = m_iSize - m_readBytes;
  if ( max == 0 )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF 2" );
    data->length( 0 );
    return data;
  }
  
  char *p = new char[ max ];
  m_stream->readRawBytes( p, max );
  
  m_readBytes += max;
  data->length( max );
  for( unsigned int i = 0; i < max; i++ )
    (*data)[i] = p[i];
  delete [] p;

  return data;
}

long KoTarStore::read( char *_buffer, unsigned long _len )
{
  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: You must open before reading" );
    return -1;
  }
  if ( m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Can not read from store that is opened for writing" );
    return -1;
  }
  
  if ( m_stream->atEnd() )
    return 0;
  
  if ( _len > m_iSize - m_readBytes )
    _len = m_iSize - m_readBytes;
  if ( _len == 0 )
    return 0;
  
  m_stream->readRawBytes( _buffer, _len );
  /*
  unsigned int len = m_in.gcount();
  if ( len != _len )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Error while reading" );
    return -1;
  }
  */
  
  m_readBytes += _len;
  
  return _len;
}

CORBA::Boolean KoTarStore::write( const KOStore::Data& data )
{
  unsigned int len = data.length();
  if ( len == 0L ) return true;
  unsigned char *p = new unsigned char[ len ];
  for( unsigned int i = 0; i < len; i++ )
    p[i] = data[i];
    
  bool ret = write( (const char*)p, len ); // see below
  
  delete [] p;

  return ret;
}

bool KoTarStore::write( const char* _data, unsigned long _len )
{
  if ( _len == 0L ) return true;

  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: You must open before writing" );
    return 0L;
  }
  if ( m_mode != KOStore::Write  )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Can not write to store that is opened for reading" );
    return 0L;
  }

  m_stream->writeRawBytes( _data, _len );
  m_iSize += _len;

  return true;
}

