// -*- c-basic-offset: 2 -*-
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

#include <koStore.h>

#include <qbuffer.h>
#include <kdebug.h>
#include <ktar.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace {
  const char* const ROOTPART = "root";
  const char* const MAINNAME = "maindoc.xml";
}

KoStore::KoStore( const QString & _filename, Mode _mode, const QCString & appIdentification )
{
  kdDebug(s_area) << "KoStore Constructor filename = " << _filename
    << " mode = " << int(_mode) << endl;

  m_pTar = new KTarGz( _filename, "application/x-gzip" );

  init( _mode ); // open the targz file and init some vars

  if ( m_bGood && _mode == Write )
      m_pTar->setOrigFileName( appIdentification );
}

KoStore::KoStore( QIODevice *dev, Mode mode )
{
  m_pTar = new KTarGz( dev );
  init( mode );
}

KoStore::~KoStore()
{
  m_pTar->close();
  delete m_pTar;
  delete m_stream;
}

bool KoStore::open( const QString & _name )
{
  m_sName = toExternalNaming( _name );

  if ( m_bIsOpen )
  {
    kdWarning(s_area) << "KoStore: File is already opened" << endl;
    return false;
  }

  if ( m_sName.length() > 512 )
  {
      kdError(s_area) << "KoStore: Filename " << m_sName << " is too long" << endl;
    return false;
  }

  if ( m_mode == Write )
  {
    kdDebug(s_area) << "KoStore: opening for writing '" << m_sName << "'" << endl;
    if ( m_strFiles.findIndex( m_sName ) != -1 ) // just check if it's there
    {
      kdWarning(s_area) << "KoStore: Duplicate filename " << m_sName << endl;
      return false;
    }

    m_strFiles.append( m_sName );
    m_iSize = 0;
    m_byteArray.resize( 0 );
    m_stream = new QBuffer( m_byteArray );
    m_stream->open( IO_WriteOnly );
  }
  else if ( m_mode == Read )
  {
    kdDebug(s_area) << "Opening for reading '" << m_sName << "'" << endl;

    const KTarEntry * entry = m_pTar->directory()->entry( m_sName );
    if ( entry == 0L )
    {
      kdWarning(s_area) << "Unknown filename " << m_sName << endl;
      return false;
    }
    if ( entry->isDirectory() )
    {
      kdWarning(s_area) << m_sName << " is a directory !" << endl;
      return false;
    }
    KTarFile * f = (KTarFile *) entry;
    m_byteArray.resize( 0 );
    delete m_stream;
    m_stream = f->device();
    m_iSize = f->size();
  }
  else
    assert( 0 );

  m_bIsOpen = true;

  return true;
}

bool KoStore::isOpen() const
{
  return m_bIsOpen;
}

void KoStore::close()
{
  kdDebug(s_area) << "KoStore: Closing" << endl;

  if ( !m_bIsOpen )
  {
    kdWarning(s_area) << "KoStore: You must open before closing" << endl;
    return;
  }

  if ( m_mode == Write )
  {
    // write the whole bytearray at once into the tar file

    kdDebug(s_area) << "Writing file " << m_sName << " into TAR archive. size "
                    << m_iSize << endl;
    if ( !m_pTar->writeFile( m_sName , "user", "group", m_iSize, m_byteArray.data() ) )
      kdWarning( s_area ) << "Failed to write " << m_sName << endl;
    m_byteArray.resize( 0 ); // save memory
  }

  delete m_stream;
  m_stream = 0L;
  m_bIsOpen = false;
}

QIODevice* KoStore::device() const
{
  if ( !m_bIsOpen )
    kdWarning(s_area) << "KoStore: You must open before asking for a device" << endl;
  if ( m_mode != Read )
    kdWarning(s_area) << "KoStore: Can not get device from store that is opened for writing" << endl;
  return m_stream;
}

QByteArray KoStore::read( unsigned long int max )
{
  QByteArray data; // Data is a QArray<char>

  if ( !m_bIsOpen )
  {
    kdWarning(s_area) << "KoStore: You must open before reading" << endl;
    data.resize( 0 );
    return data;
  }
  if ( m_mode != Read )
  {
    kdError(s_area) << "KoStore: Can not read from store that is opened for writing" << endl;
    data.resize( 0 );
    return data;
  }

  if ( m_stream->atEnd() )
  {
    data.resize( 0 );
    return data;
  }

  if ( max > m_iSize - m_stream->at() )
    max = m_iSize - m_stream->at();
  if ( max == 0 )
  {
    data.resize( 0 );
    return data;
  }

  char *p = new char[ max ];
  m_stream->readBlock( p, max );

  data.setRawData( p, max );
  return data;
}

Q_LONG KoStore::write( const QByteArray& data )
{
  return write( data.data(), data.size() ); // see below
}

Q_LONG KoStore::read( char *_buffer, Q_ULONG _len )
{
  if ( !m_bIsOpen )
  {
    kdError(s_area) << "KoStore: You must open before reading" << endl;
    return -1;
  }
  if ( m_mode != Read )
  {
    kdError(s_area) << "KoStore: Can not read from store that is opened for writing" << endl;
    return -1;
  }

  if ( m_stream->atEnd() )
    return 0;

  if ( _len > m_iSize - m_stream->at() )
    _len = m_iSize - m_stream->at();
  if ( _len == 0 )
    return 0;

  return m_stream->readBlock( _buffer, _len );
}

Q_LONG KoStore::write( const char* _data, Q_ULONG _len )
{
  if ( _len == 0L ) return 0;

  if ( !m_bIsOpen )
  {
    kdError(s_area) << "KoStore: You must open before writing" << endl;
    return 0L;
  }
  if ( m_mode != Write  )
  {
    kdError(s_area) << "KoStore: Can not write to store that is opened for reading" << endl;
    return 0L;
  }

  int nwritten = m_stream->writeBlock( _data, _len );
  Q_ASSERT( nwritten == (int)_len );
  m_iSize += nwritten;

  return nwritten;
}

bool KoStore::embed( const QString &dest, KoStore &store, const QString &src )
{
  if ( dest == ROOTPART )
  {
    kdError(s_area) << "KoStore: cannot embed root part" << endl;
    return false;
  }

  // Find the destination directory corresponding to the part to be embedded.

  QString destDir;

  destDir = toExternalNaming( dest );
  if ( destDir.mid( destDir.length() - sizeof(MAINNAME) + 1 ) == MAINNAME )
  {
    destDir = destDir.left( destDir.length() - sizeof(MAINNAME) + 1 );
  }
  else
  {
    kdError(s_area) << "KoStore: cannot embed to a part called " << destDir << endl;
    return false;
  }

  // Find the source directory corresponding to the part to be embedded.

  QString srcDir;

  srcDir = store.toExternalNaming( src );
  if ( srcDir.mid( srcDir.length() - sizeof(MAINNAME) + 1 ) == MAINNAME )
  {
    srcDir = srcDir.left( srcDir.length() - sizeof(MAINNAME) + 1 );
  }
  else
  {
    kdError(s_area) << "KoStore: cannot embed from a part called " << srcDir << endl;
    return false;
  }

  // Now recurse into the embedded part, addings its top level contents to our tar.

  kdDebug(s_area) << "KoStore: embedding " << srcDir << " in " << destDir << endl;
  const KTarEntry *entry;
  if ( src == ROOTPART )
  {
    entry = store.m_pTar->directory();
  }
  else
  {
    entry = store.m_pTar->directory()->entry( srcDir );
  }
  QStringList entries = dynamic_cast<const KTarDirectory *>( entry )->entries();
  unsigned i;

  for ( i = 0; i < entries.count(); i++ )
  {
    if ( store.m_pTar->directory()->entry( srcDir + entries[i] )->isDirectory() )
    {
      // Recurse to get the files in the next level down.

      if ( embed( destDir + entries[i] + "/" + MAINNAME,
                  store,
                  srcDir + entries[i] + "/" + MAINNAME ) )
      {
        kdDebug(s_area) << "KoStore: embedded " << srcDir << " in " << destDir << endl;
      }
      else
      {
        break;
      }
    }
    else
    {
      kdDebug(s_area) << "KoStore: is file " << endl;
      if ( ( open( destDir + entries[i] ) && store.open( srcDir + entries[i] ) ) )
      {
        kdDebug(s_area) << "KoStore: embedding file " << entries[i] << endl;
        long length = store.size();
        write( store.read( length ));
        store.close();
        close();
      }
      else
      {
        break;
      }
    }
  }
  return i == entries.count();
}

QIODevice::Offset KoStore::size() const
{
  if ( !m_bIsOpen )
  {
    kdWarning(s_area) << "KoStore: You must open before asking for a size" << endl;
    return static_cast<Q_ULONG>(-1);
  }
  if ( m_mode != Read )
  {
    kdWarning(s_area) << "KoStore: Can not get size from store that is opened for writing" << endl;
    return static_cast<Q_ULONG>(-1);
  }
  return m_iSize;
}

bool KoStore::enterDirectory( const QString& directory )
{
  int pos;
  bool success = true;
  QString tmp( directory );

  while ( ( pos = tmp.find( '/' ) ) != -1 &&
          ( success = enterDirectoryInternal( tmp.left( pos ) ) ) )
          tmp = tmp.mid( pos + 1 );

  if ( success && !tmp.isEmpty() )
    return enterDirectoryInternal( tmp );
  return success;
}

bool KoStore::leaveDirectory()
{
  if ( m_currentPath.isEmpty() )
    return false;

  m_currentPath.pop_back();

  if ( m_currentPath.isEmpty() )
    m_currentDir = 0;
  else {
    m_currentDir = dynamic_cast<const KArchiveDirectory*>( m_pTar->directory()->entry( expandEncodedDirectory( currentPath() ) ) );
    Q_ASSERT( m_currentDir );
  }
  return true;
}

QString KoStore::currentPath() const
{
  QString path;
  QStringList::ConstIterator it = m_currentPath.begin();
  QStringList::ConstIterator end = m_currentPath.end();
  for ( ; it != end; ++it ) {
    path += *it;
    path += '/';
  }
  return path;
}

bool KoStore::at( QIODevice::Offset pos )
{
  return m_stream->at( pos );
}

QIODevice::Offset KoStore::at() const
{
  return m_stream->at();
}

bool KoStore::atEnd() const
{
  return m_stream->atEnd();
}

// See the specification for details of what this function does.
QString KoStore::toExternalNaming( const QString & _internalNaming )
{
  if ( _internalNaming == ROOTPART )
    return expandEncodedDirectory( currentPath() ) + MAINNAME;

  QString intern;
  if ( _internalNaming.startsWith( "tar:/" ) ) // absolute reference
    intern = _internalNaming.mid( 5 ); // remove protocol
  else
    intern = currentPath() + _internalNaming;

  return expandEncodedPath( intern );
}

QString KoStore::expandEncodedPath( QString intern )
{
  QString result;
  int pos;
  while ( ( pos = intern.find( '/' ) ) != -1 ) {
    if ( QChar(intern.at(0)).isDigit() )
      result += "part";
    result += intern.left( pos + 1 ); // copy numbers (or "pictures") + "/"
    intern = intern.mid( pos + 1 ); // remove the dir we just processed
  }

  // Now process the filename. If the first character is numeric, we have
  // a main document.
  if ( QChar(intern.at(0)).isDigit() )
  {
    // If this is the first part name, check if we have a store with
    // old-style names.
    if ( ( m_namingVersion == NAMING_VERSION_2_2 ) &&
         ( m_mode == Read ) &&
         ( m_pTar->directory()->entry( result + "part" + intern + ".xml" ) ) )
      m_namingVersion = NAMING_VERSION_2_1;

    if ( m_namingVersion == NAMING_VERSION_2_1 )
      result = result + "part" + intern + ".xml";
    else
      result = result + "part" + intern + "/" + MAINNAME;
  }
  else
    result += intern;
  return result;
}

QString KoStore::expandEncodedDirectory( QString intern )
{
  QString result;
  int pos;
  while ( ( pos = intern.find( '/' ) ) != -1 ) {
    if ( QChar(intern.at(0)).isDigit() )
      result += "part";
    result += intern.left( pos + 1 ); // copy numbers (or "pictures") + "/"
    intern = intern.mid( pos + 1 ); // remove the dir we just processed
  }

  if ( QChar(intern.at(0)).isDigit() )
    result += "part";
  result += intern;
  return result;
}

void KoStore::init( Mode _mode )
{
  d = 0;
  m_bIsOpen = false;
  m_mode = _mode;
  m_stream = 0;
  m_currentDir = 0;

  m_bGood = m_pTar->open( _mode == Write ? IO_WriteOnly : IO_ReadOnly );

  if ( m_bGood && _mode == Read )
      m_bGood = m_pTar->directory() != 0;

  // Assume new style names.
  m_namingVersion = NAMING_VERSION_2_2;
}

bool KoStore::enterDirectoryInternal( const QString& directory )
{
  if ( m_mode == Read ) {
    if ( !m_currentDir ) {
      m_currentDir = m_pTar->directory(); // initialize
      Q_ASSERT( m_currentPath.isEmpty() );
    }
    const KArchiveEntry *entry = m_currentDir->entry( expandEncodedDirectory( directory ) );
    if ( entry && entry->isDirectory() ) {
      m_currentPath.append( directory );
      m_currentDir = dynamic_cast<const KArchiveDirectory*>( entry );
      return m_currentDir != 0;
    }
    return false;
  }
  else { // Write, no checking here
    m_currentPath.append( directory );
    return true;
  }
}
