/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kwdrag.h"
#include "defs.h"
#include <kdebug.h>
#include <kotextobject.h>


bool KWTextDrag::provides( QMimeSource* e, const char* acceptMimeType, QCString &returnedTypeMime)
{
    const char* fmt;
    for (int i=0; (fmt = e->format(i)); i++) {
	if ( QString( fmt ).startsWith( acceptMimeType ) )
        {
            returnedTypeMime = fmt;
	    return true;
        }
    }
    return false;
}

bool KWTextDrag::canDecode( QMimeSource* e )
{
    QCString tmp;
    if ( KWTextDrag::provides( e, KoTextObject::acceptSelectionMimeType(), tmp ) )
       return true;
    return QTextDrag::canDecode(e);
}

const char * KWTextDrag::selectionMimeType()
{
    return "application/vnd.oasis.opendocument.text";
}

int KWTextDrag::decodeFrameSetNumber( QMimeSource *e )
{
    QByteArray a =  e->encodedData("application/x-kword-framesetnumber");
    if(!a.isEmpty())
        return QCString(a, a.size()+1 ).toInt();
    else
        return -1;
}
