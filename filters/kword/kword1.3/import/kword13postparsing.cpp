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

#include <koStore.h>

#include "kword13document.h"
#include "kword13postparsing.h"

KWord13PostParsing::KWord13PostParsing(void) : m_kwordDocument( 0 )
{
}

KWord13PostParsing::~KWord13PostParsing(void)
{
    delete m_kwordDocument;
}


bool KWord13PostParsing::postParse( KoStore* store, KWord13Document& doc )
{
    // Do check if it is the same document if clled twice
    m_kwordDocument = &doc;
    
    // ### TODO
    
    return 1;
}
