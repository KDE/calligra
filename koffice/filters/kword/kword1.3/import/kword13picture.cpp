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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qfile.h>

#include <kdebug.h>
#include <ktempfile.h>

#include <KoStore.h>
#include <KoStoreDevice.h>

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
 
    m_tempFile = new KTempFile( QString::null, ".bin" );
    m_tempFile->setAutoDelete( true );
 
    if ( ! store->extractFile( m_storeName, m_tempFile->name() ) )   
    {
        kdWarning(30520) << "Could not write temporary file!" << endl;
        delete m_tempFile;
        m_tempFile = 0;
        m_valid = false;
    }
    else
    {
        m_valid = true;
    }
    return m_valid;
}

QString KWord13Picture::getOasisPictureName( void ) const
{
    if ( ! m_valid || ! m_tempFile )
        return QString::null;
        
    // We need a 32 digit hex value of the picture number
    // Please note: it is an exact 32 digit value, truncated if the value is more than 512 bits wide. :-)
    QString number;
    number.fill('0',32);
    // ### TODO: have a real counter instead of using the pointers
    number += QString::number( (long long)( (void*) m_tempFile ) , 16 ); // in hex

    QString strExtension( m_storeName.lower() );
    const int result = m_storeName.findRev( '.' );
    if ( result >= 0 )
    {
        strExtension = m_storeName.mid( result );
    }
    
    QString ooName( "Pictures/" );
    ooName += number.right( 32 );
    ooName += strExtension;

    return ooName;
}
