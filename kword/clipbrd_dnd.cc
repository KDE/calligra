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

#include "clipbrd_dnd.h"
#include "clipbrd_dnd.moc"
#include "parag.h"
#include "defs.h"

static const char *MimeTypes[] = {"text/plain", "text/html", MIME_TYPE, 0};

/******************************************************************/
/* Class: KWordDrag                                               */
/******************************************************************/

/*================================================================*/
KWordDrag::KWordDrag( QWidget *dragSource, const char *name )
    : QDragObject( dragSource, name ), kword(), plain(), html()
{
}

/*================================================================*/
void KWordDrag::setPlain( const QString &_plain )
{
    plain = _plain;
}

/*================================================================*/
void KWordDrag::setKWord( const QString &_kword )
{
    kword = _kword;
}

/*================================================================*/
void KWordDrag::setHTML( const QString &_html )
{
    html = _html;
}

/*================================================================*/
const char *KWordDrag::format( int i ) const
{
    for ( int j = 0; MimeTypes[ j ]; j++ )
    {
        if ( i == j )
            return MimeTypes[ j ];
    }

    return 0L;
}

/*================================================================*/
QByteArray KWordDrag::encodedData( const char *mime ) const
{
    QCString str;

    if ( QString( mime ) == QString::fromLatin1(MimeTypes[ 0 ]) )
        str = plain.ascii();
    else if ( QString( mime ) == QString::fromLatin1(MimeTypes[ 1 ]) )
        str = html.ascii();
    else if ( QString( mime ) == QString::fromLatin1(MimeTypes[ 2 ]) )
        str = kword.ascii();

    return str;
}

/*================================================================*/
bool KWordDrag::canDecode( QMimeSource* e )
{
    for ( unsigned int i = 0; MimeTypes[ i ]; i++ )
    {
        if ( e->provides( QString::fromLatin1(MimeTypes[ i ]) ) )
            return true;
    }
    return false;
}

/*================================================================*/
bool KWordDrag::decode( QMimeSource* e, QString& s )
{
    for ( unsigned int i = 0; MimeTypes[ i ]; i++ )
    {
        QByteArray ba = e->encodedData( QString::fromLatin1(MimeTypes[ i ]) );
        if ( ba.size() )
        {
            s = QString( ba );
            return true;
        }
    }

    return false;
}

