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

#include <qstring.h>
#include <qdict.h>

#include <kdebug.h>

#include <koStore.h>

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
        
    for ( QDictIterator<KWord13Picture> it( m_kwordDocument->m_pictureDict ) ; it.current(); ++it )
    {
        kdDebug(30520) << "Loading... " << it.currentKey() << endl;
        if ( ! it.current()->loadPicture( store ) )
        {
            kdWarning(30520) << "Could not load picture!" << endl;
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
