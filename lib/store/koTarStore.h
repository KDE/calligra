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

#ifndef __koTarStore_h__
#define __koTarStore_h__

#include <qstring.h>
#include <qstringlist.h>

#include "koStore.h"

#include <qdatastream.h>

class KTarGz;

/**
 * Saves and loads koffice documents using a tar file called "the tar store".
 * For method documentation, see koStore.h
 */
class KoTarStore : public KoStore
{
public:
  /**
   * Creates a Tar Store (i.e. a file on the hard disk, to save a document)
   * if _mode is KoStore::Write
   * Opens a Tar Store for reading if _mode is KoStore::Read.
   */
  KoTarStore( const QString & _filename, Mode _mode );
  virtual ~KoTarStore();

  virtual bool open( const QString & name, const QCString &_mime_type = "" );
  virtual void close();
  virtual QByteArray read( long unsigned int max );
  virtual bool write( const QByteArray& _data );

  virtual bool write( const char* _data, unsigned long _len );
  virtual long read( char *_buffer, unsigned long _len );

  /**
   * @return the size of the currently opened file, -1 on error.
   * Can be used as an argument for the read methods, for instance
   */
  virtual long size() const;

  virtual bool bad() { return !m_bGood; } // :)

protected:
  /**
   * Conversion routine
   * @param _internalNaming name used internally : "root", "tar:/0", ...
   * @return the name used in the file, more user-friendly ("maindoc.xml", "part0.xml", ...)
   */
  static QString toExternalNaming( const QString & _internalNaming );

  Mode m_mode;

  // Store the filenames (with full path inside the archive)  when writing, to avoid duplicates
  QStringList m_strFiles;

  // Current filename (between an open() and a close())
  QString m_sName;
  // Current size of the file named m_sName
  unsigned long m_iSize;

  KTarGz * m_pTar;
  QByteArray m_byteArray;
  QDataStream * m_stream;

  bool m_bIsOpen;
  bool m_bGood;

  int m_readBytes;
};

#endif
