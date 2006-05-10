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

#include <QString>
#include <q3dict.h>

#include <kdebug.h>

#include <KoStore.h>

#include "kword13picture.h"
#include "kword13document.h"
#include "kword13postparsing.h"

KWord13PostParsing::KWord13PostParsing(void) : m_kwordDocument( 0 )
{
}

KWord13PostParsing::~KWord13PostParsing(void)
{
}

bool KWord13PostParsing::postParsePictures( KoStore* store )
{
    if ( ! m_kwordDocument )
        return false;
        
    for ( Q3DictIterator<KWord13Picture> it( m_kwordDocument->m_pictureDict ) ; it.current(); ++it )
    {
        kDebug(30520) << "Loading... " << it.currentKey() << endl;
        if ( ! it.current()->loadPicture( store ) )
        {
            kWarning(30520) << "Could not load picture!" << endl;
            return false;
        }
    }
    return true;
}


bool KWord13PostParsing::postParse( KoStore* store, KWord13Document& doc )
{
    // ### TODO: check if it is the same document if called twice
    m_kwordDocument = &doc;
    
    // ### TODO: return value
    postParsePictures( store );
    // ### TODO
    
    return 1;
}
