/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <david@mandrakesoft.com>

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

#include "koDirectoryStore.h"
#include <qfile.h>
#include <qdir.h>
#include <kdebug.h>

KoDirectoryStore::KoDirectoryStore( const QString& path, Mode _mode )
    : m_basePath( path ), m_file( 0L )
{
    if ( !m_basePath.endsWith("/") )
        m_basePath += '/';
    m_currentPath = m_basePath;
    m_bGood = init( _mode );
}

KoDirectoryStore::~KoDirectoryStore()
{
    delete m_file;
}

bool KoDirectoryStore::init( Mode _mode )
{
    KoStore::init( _mode );
    QDir dir( m_basePath );
    if ( !dir.exists() )
        return false;
    return true;
}

bool KoDirectoryStore::openReadOrWrite( const QString& name, int iomode )
{
    kdDebug(s_area) << "KoDirectoryStore::openReadOrWrite m_currentPath=" << m_currentPath << " name=" << name << endl;
    m_file = new QFile( m_currentPath + name );
    if ( !m_file->open( iomode ) )
    {
        delete m_file;
        m_file = 0L;
        return false;
    }
    return true;
}

bool KoDirectoryStore::closeReadOrWrite()
{
    Q_ASSERT( m_file );
    if ( m_file )
    {
        delete m_file; // calls QFile::close()
        m_file = 0L;
        return true;
    }
    return false;
}

bool KoDirectoryStore::enterRelativeDirectory( const QString& dirName )
{
    QDir origDir( m_currentPath );
    m_currentPath += dirName;
    if ( !m_currentPath.endsWith("/") )
        m_currentPath += '/';
    kdDebug() << "KoDirectoryStore::enterRelativeDirectory m_currentPath now " << m_currentPath << endl;
    QDir newDir( m_currentPath );
    if ( newDir.exists() )
        return true;
    // Dir doesn't exist. If reading -> error. If writing -> create.
    if ( mode() == Write && origDir.mkdir( dirName ) )
        return true;
    return false;
}

bool KoDirectoryStore::enterAbsoluteDirectory( const QString& path )
{
    m_currentPath = m_basePath + path;
    QDir newDir( m_currentPath );
    Q_ASSERT( newDir.exists() ); // We've been there before, therefore it must exist.
    return newDir.exists();
}

bool KoDirectoryStore::fileExists( const QString& absPath )
{
    return QFile::exists( m_basePath + absPath );
}
