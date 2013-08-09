/*
   This file is part of the KDE libraries
   Copyright (C) 2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kmemfile_p.h"

#ifndef QT_NO_SHAREDMEMORY

#include <QtCore/QSharedMemory>
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QCoreApplication>

class KMemFile::Private
{
public:
  struct sharedInfoData {
    int  shmCounter;
    qint64 shmDataSize;

    sharedInfoData() {
      memset ( this, 0, sizeof ( *this ) );
    }
  };
  Private ( KMemFile *_parent ) : readWritePos ( 0 ), shmDataSize ( 0 ), parent ( _parent ) {}

  QString getShmKey ( int iCounter = -1 );
  static QString getShmKey ( const QString &filename, int iCounter = -1 );
  bool loadContentsFromFile();
  void close();

  QString filename;
  QSharedMemory shmInfo;
  QSharedMemory shmData;
  qint64 readWritePos;
  qint64 shmDataSize;

  KMemFile *parent;
};

QString KMemFile::Private::getShmKey ( int iCounter )
{
  return getShmKey ( filename, iCounter );
}

QString KMemFile::Private::getShmKey ( const QString &filename, int iCounter )
{
  QByteArray tmp = QString ( QDir ( filename ).canonicalPath() + QString::number ( iCounter ) ).toUtf8();
  return QString::fromLatin1 ( QCryptographicHash::hash ( tmp, QCryptographicHash::Sha1 ) );
}

bool KMemFile::Private::loadContentsFromFile()
{
  QFile f ( filename );
  if ( !f.exists() ) {
    close();
    parent->setErrorString(QCoreApplication::translate("", "File %1 does not exist").arg(filename));
    return false;
  }
  if ( !f.open ( QIODevice::ReadOnly ) ) {
    close();
    parent->setErrorString(QCoreApplication::translate("", "Cannot open %1 for reading").arg(filename));
    return false;
  }

  sharedInfoData *infoPtr = static_cast<sharedInfoData*> ( shmInfo.data() );

  infoPtr->shmDataSize = f.size();
  shmData.setKey ( getShmKey ( infoPtr->shmCounter ) );
  if ( !shmData.create ( infoPtr->shmDataSize ) ) {
    close();
    parent->setErrorString(QCoreApplication::translate("", "Cannot create memory segment for file %1").arg(filename));
    return false;
  }
  shmData.lock();
  qint64 size = 0;
  qint64 bytesRead;
  char *data = static_cast<char*> ( shmData.data() );
  bytesRead = f.read ( data, infoPtr->shmDataSize );
  if ( bytesRead != infoPtr->shmDataSize ) {
    close();
    parent->setErrorString(QCoreApplication::translate("", "Could not read data from %1 into shm").arg(filename));
    return false;
  }
  shmDataSize = size;
  shmData.unlock();
  return true;
}

void KMemFile::Private::close()
{
  shmData.unlock();
  shmData.detach();
  shmInfo.unlock();
  shmInfo.detach();
  readWritePos = 0;
  shmDataSize = 0;
}

KMemFile::KMemFile ( const QString &filename, QObject *parent )
    : QIODevice ( parent ), d ( new Private ( this ) )
{
  d->filename = filename;
}

KMemFile::~KMemFile()
{
  close();
  delete d;
}

void KMemFile::close ()
{
  QIODevice::close();
  if ( !isOpen() )
    return;
  d->close();
}

bool KMemFile::isSequential () const
{
  return false;
}

bool KMemFile::open ( OpenMode mode )
{
  if ( isOpen() ) {
    QIODevice::open ( mode );
    return false;
  }

  if ( mode != QIODevice::ReadOnly ) {
    setErrorString(QCoreApplication::translate("", "Only 'ReadOnly' allowed"));
    return false;
  }

  if ( !QFile::exists ( d->filename ) ) {
    setErrorString(QCoreApplication::translate("", "File %1 does not exist").arg(d->filename));
    return false;
  }

  QSharedMemory lock ( QDir ( d->filename ).canonicalPath() );
  lock.lock();

  Private::sharedInfoData *infoPtr;
  d->shmInfo.setKey ( d->getShmKey() );
  // see if it's already in memory
  if ( !d->shmInfo.attach ( QSharedMemory::ReadWrite ) ) {
    if ( !d->shmInfo.create ( sizeof ( Private::sharedInfoData ) ) ) {
      lock.unlock();
      setErrorString(QCoreApplication::translate("", "Cannot create memory segment for file %1").arg(d->filename));
      return false;
    }
    d->shmInfo.lock();
    // no -> create it
    infoPtr = static_cast<Private::sharedInfoData*> ( d->shmInfo.data() );
    memset ( infoPtr, 0, sizeof ( Private::sharedInfoData ) );
    infoPtr->shmCounter = 1;
    if ( !d->loadContentsFromFile() ) {
      d->shmInfo.unlock();
      d->shmInfo.detach();
      lock.unlock();
      return false;
    }
  } else {
    d->shmInfo.lock();
    infoPtr = static_cast<Private::sharedInfoData*> ( d->shmInfo.data() );
    d->shmData.setKey ( d->getShmKey ( infoPtr->shmCounter ) );
    if ( !d->shmData.attach ( QSharedMemory::ReadOnly ) ) {
      if ( !d->loadContentsFromFile() ) {
        d->shmInfo.unlock();
        d->shmInfo.detach();
        lock.unlock();
        return false;
      }
    }
  }
  d->shmDataSize = infoPtr->shmDataSize;
  d->shmInfo.unlock();
  lock.unlock();

  setOpenMode ( mode );
  return true;
}

bool KMemFile::seek ( qint64 pos )
{
  if ( d->shmDataSize < pos ) {
    setErrorString ( QCoreApplication::translate("", "Cannot seek past eof" ) );
    return false;
  }
  d->readWritePos = pos;
  QIODevice::seek ( pos );
  return true;
}

qint64 KMemFile::size () const
{
  return d->shmDataSize;
}

qint64 KMemFile::readData ( char * data, qint64 maxSize )
{
  if ( ( openMode() & QIODevice::ReadOnly ) == 0 )
    return -1;

  qint64 maxRead = size() - d->readWritePos;
  qint64 bytesToRead = qMin ( maxRead, maxSize );
  const char *src = static_cast<const char*> ( d->shmData.data() );
  memcpy ( data, &src[d->readWritePos], bytesToRead );
  d->readWritePos += bytesToRead;
  return bytesToRead;
}

qint64 KMemFile::writeData ( const char *, qint64 )
{
  return -1;
}

void KMemFile::fileContentsChanged ( const QString &filename )
{
  QSharedMemory lock ( QDir ( filename ).canonicalPath() );
  lock.lock();

  QSharedMemory shmData ( Private::getShmKey ( filename ) );
  if ( !shmData.attach() )
    return;
  shmData.lock();
  Private::sharedInfoData *infoPtr = static_cast<Private::sharedInfoData*> ( shmData.data() );
  infoPtr->shmCounter++;
  infoPtr->shmDataSize = 0;
  shmData.unlock();
}

#endif //QT_NO_SHAREDMEMORY
