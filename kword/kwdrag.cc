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
#include "kwdrag.moc"
#include "defs.h"
#include <kdebug.h>
#include <kotextobject.h>

/******************************************************************/
/* Class: KWTextDrag                                               */
/******************************************************************/

KWTextDrag::KWTextDrag( QWidget *dragSource, const char *name )
    : QTextDrag( dragSource, name )
{
}

QByteArray KWTextDrag::encodedData( const char *mime ) const
{
    if ( ( strcmp( selectionMimeType(), mime ) == 0 )|| QString( mime ).startsWith( KoTextObject::acceptSelectionMimeType()))
        return kword;
    else if( strcmp( "application/x-kword-framesetnumber", mime ) == 0)
    {
        QByteArray a;
        QCString s (  QString::number(m_framesetNumber).local8Bit() );
	a.resize( s.length() + 1 ); // trailing zero
	memcpy( a.data(), s.data(), s.length() + 1 );
        return a;
    }
    else
        return QTextDrag::encodedData(mime);
}

bool KWTextDrag::provides( QMimeSource* e , const char* mimeType, const char* acceptMimeType, QString &returnedTypeMime)
{
    const char* fmt;
    for (int i=0; (fmt = e->format(i)); i++) {
	if ( !qstricmp(mimeType,fmt) || QString( fmt ).startsWith( acceptMimeType ))
        {
            returnedTypeMime = fmt;
	    return TRUE;
        }
    }
    return FALSE;
}

bool KWTextDrag::canDecode( QMimeSource* e )
{
    QString tmp;
    if ( KWTextDrag::provides( e, selectionMimeType(), KoTextObject::acceptSelectionMimeType(),tmp ) )
       return true;
    return QTextDrag::canDecode(e);
}

const char* KWTextDrag::format( int i ) const
{
    if ( i < 4 ) // HACK, but how to do otherwise ??
        return QTextDrag::format(i);
    else if ( i == 4 )
        return selectionMimeType();
    else if ( i == 5 )
        return "application/x-kword-framesetnumber";
    else return 0;
}

const char * KWTextDrag::selectionMimeType()
{
    return "application/vnd.oasis.opendocument.text";
}

void KWTextDrag::setFrameSetNumber( int number )
{
    m_framesetNumber = number;
}

int KWTextDrag::decodeFrameSetNumber( QMimeSource *e )
{
    QByteArray a =  e->encodedData("application/x-kword-framesetnumber");
    if(!a.isEmpty())
        return QCString(a, a.size()+1 ).toInt();
    else
        return -1;
}
