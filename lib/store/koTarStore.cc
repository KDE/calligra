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

KoTarStore::KoTarStore( const QString & _filename, KOStore::Mode _mode )
{
  m_bIsOpen = false;
  m_mode = _mode;
  m_stream = 0L;
  
  kdebug( KDEBUG_INFO, 30002, "KoTarStore Constructor filename = %s mode = %d", _filename.data(), _mode);

  m_pTar = new KTar( _filename );

  m_bGood = m_pTar->open( _mode == KOStore::Write ? IO_WriteOnly : IO_ReadOnly );
}

KoTarStore::~KoTarStore()
{
  kdebug( KDEBUG_INFO, 30002, "###################### DESTRUCTOR ####################" );
  m_pTar->close();
  delete m_pTar;
  if ( m_stream )
    delete m_stream;
}

QString KoTarStore::toExternalNaming( const QString & _internalNaming )
{
  // "root" is the main document, let's save it as "maindoc.xml"
  if (_internalNaming == "root") 
    return "maindoc.xml";

  /*
  // tar:/0 is saved as part0.xml
  // tar:/0/1 is saved as part0/part1.xml
  // tar:/0/1/pictures/picture0.png is saved as part0/part1/pictures/picture0.png
  */
  if ( _internalNaming.left(5) == "tar:/" )
  {
    QString intern( _internalNaming.mid( 5 ) ); // remove protocol
    QString result( "" );
    int pos;
    while ( ( pos = intern.find( '/' ) ) != -1 ) {
      if ( QChar(intern.at(0)).isDigit() )
        result += "part";
      result += intern.left( pos + 1 ); // copy numbers (or "pictures") + "/"
      intern = intern.mid( pos + 1 ); // remove the dir we just processed
    }
    // now process the filename
    if ( QChar(intern.at(0)).isDigit() )
      result = result + "part" + intern + ".xml";
    else
      result += intern;

    return result;
  }

  return _internalNaming;
}

bool KoTarStore::open( const QString & _name, const QCString& /*_mime_type*/ )
{
  m_sName = toExternalNaming( _name );

  if ( m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: File is already opened" );
    return false;
  }
    
  if ( m_sName.length() > 512 )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Filename %s is too long", m_sName.latin1() );
    return false;
  }
  
  if ( m_mode == KOStore::Write )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: opening for writing '%s'", m_sName.latin1());
    if ( m_strFiles.findIndex( m_sName ) != -1 ) // just check if it's there
    {
      kdebug( KDEBUG_INFO, 30002, "KoTarStore: Duplicate filename %s", m_sName.latin1() );
      return false;
    }
    
    m_strFiles.append( m_sName );
    m_iSize = 0;
  }
  else if ( m_mode == KOStore::Read )
  { 
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
    // warning, m_byteArray can be bigger than f->data().size() (if a previous file was bigger)
    // this is why we never use m_byteArray.size()
    m_iSize = f->data().size(); 
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
  
    kdebug( KDEBUG_INFO, 30002, "Writing file %s into TAR archive. size %d.",
          m_sName.latin1(), m_iSize );
    m_pTar->writeFile( m_sName , "user", "group", m_iSize, m_byteArray.data() );
  }

  delete m_stream;
  m_stream = 0L;
  m_bIsOpen = false;
}

KOStore::Data KoTarStore::read( unsigned long int max )
{
  KOStore::Data data; // Data is a QArray<char> 

  if ( !m_bIsOpen )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: You must open before reading" );
    data.resize( 0 );
    return data;
  }
  if ( m_mode != KOStore::Read )
  {
    kdebug( KDEBUG_INFO, 30002, "KoTarStore: Can not read from store that is opened for writing" );
    data.resize( 0 );
    return data;
  }
  
  if ( m_stream->atEnd() )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF" );
    data.resize( 0 );
    return data;
  }
  
  if ( max > m_iSize - m_readBytes )
    max = m_iSize - m_readBytes;
  if ( max == 0 )
  {
    kdebug( KDEBUG_INFO, 30002, "EOF 2" );
    data.resize( 0 );
    return data;
  }
  
  char *p = new char[ max ];
  m_stream->readRawBytes( p, max );
  
  m_readBytes += max;
  data.setRawData( p, max );
  /*
  data->length( max );
  for( unsigned int i = 0; i < max; i++ )
    (*data)[i] = p[i];
  */
  //  delete [] p;  setRawData is a shallow copy

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
  
  m_readBytes += _len;
  
  return _len;
}

bool KoTarStore::write( const KOStore::Data& data )
{
  unsigned int len = data.size();
  if ( len == 0L ) return true; // nothing to do
  return write( data.data(), len ); // see below
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

