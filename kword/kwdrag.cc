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

/******************************************************************/
/* Class: KWTextDrag                                               */
/******************************************************************/

KWTextDrag::KWTextDrag( QWidget *dragSource, const char *name )
    : QTextDrag( dragSource, name )
{
}

QByteArray KWTextDrag::encodedData( const char *mime ) const
{
    if ( strcmp( selectionMimeType(), mime ) == 0 )
        return kword;
    else
        return QTextDrag::encodedData(mime);
}

bool KWTextDrag::canDecode( QMimeSource* e )
{
    if ( e->provides( selectionMimeType() ) )
       return true;
    return QTextDrag::canDecode(e);
}

const char* KWTextDrag::format( int i ) const
{
    if ( i < 4 ) // HACK, but how to do otherwise ??
        return QTextDrag::format(i);
    else if ( i == 4 )
        return selectionMimeType();
    else return 0;
}

const char * KWTextDrag::selectionMimeType()
{
    return "application/x-kword-textselection";
}

/******************************************************************/
/* Class: KWDrag                                                  */
/******************************************************************/
KWDrag::KWDrag( QWidget *dragSource, const char *name )
    : QDragObject( dragSource, name )
{
}

QByteArray KWDrag::encodedData( const char *mime ) const
{
    if ( strcmp( mimeType(), mime ) == 0 )
        return kword;
    else
        kdWarning() << "KWDrag: unsupported type " << mime << " requested" << endl;
}

bool KWDrag::canDecode( QMimeSource* e )
{
    return e->provides( mimeType() );
}

const char* KWDrag::format( int i ) const
{
    if ( i == 0 )
        return mimeType();
    else return 0;
}

const char * KWDrag::mimeType()
{
    return "application/x-kword-selection";
}
