//

/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <qfile.h>

#include <kdebug.h>
#include <ktempfile.h>

#include <koStore.h>
#include <koStoreDevice.h>

#include "kword13picture.h"
    
KWord13Picture::KWord13Picture( void ) : m_tempFile( 0 ), m_valid( false )
{
    
}

KWord13Picture::~KWord13Picture( void )
{
    delete m_tempFile;
}

bool KWord13Picture::loadPicture( KoStore* store )
{
    kdDebug(30520) << "Loading picture: " << m_storeName << endl;
    if ( ! store->open( m_storeName ) )
    {
        kdWarning(30520) << "Cannot load: " << m_storeName << endl;
    }
    KoStoreDevice ioPicture( store );
    ioPicture.open( IO_ReadOnly );
    QByteArray array ( ioPicture.readAll() );
    ioPicture.close();
    store->close();
    
    if ( array.isNull() )
    {
        kdWarning(30520) << "Null picture read!" << endl;
        return false;
    }

    m_tempFile = new KTempFile( QString::null, ".bin" );
    m_tempFile->setAutoDelete( true );
    
    QFile file ( m_tempFile->name() );
    if ( ! file.open( IO_WriteOnly ) )
    {
        kdWarning(30520) << "Xan write temporary file!" << endl;
        delete m_tempFile;
        m_tempFile = false;
        m_valid = false;
        return false;
    }
    file.writeBlock( array );
    file.close();
    m_valid = true;
    return true;
}

