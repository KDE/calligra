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
#include "defs.h"

static const char *MimeTypes[2] = { MIME_TYPE, "text/html" };

/******************************************************************/
/* Class: KWDrag                                               */
/******************************************************************/

/*================================================================*/
KWDrag::KWDrag( QWidget *dragSource, const char *name )
    : QTextDrag( dragSource, name )
{
}

/*================================================================*/
void KWDrag::setPlain( const QString &_plain )
{
    setText(_plain);
    plain = _plain;
}

/*================================================================*/
void KWDrag::setKWord( const QString &_kword )
{
    kword = _kword;
}

/*================================================================*/
void KWDrag::setHTML( const QString &_html )
{
    html = _html;
}

/*================================================================*/
QByteArray KWDrag::encodedData( const char *mime ) const
{
    if ( strcmp(mime, MimeTypes[ 1 ]) == 0 )
    {
        KWDrag *non_const_this = const_cast<KWDrag *>(this);
        non_const_this->setText(html);
    }
    else if ( strcmp( mime, MimeTypes[ 0 ]) == 0 )
    {
        QCString result = kword.utf8();
        return result;
    }
    else
    {
        KWDrag *non_const_this = const_cast<KWDrag *>(this);
        non_const_this->setText(plain);
    }

    return QTextDrag::encodedData(mime);
}

/*================================================================*/
bool KWDrag::canDecode( QMimeSource* e )
{
    if ( e->provides( MimeTypes[ 0 ] ) )
       return true;
    return QTextDrag::canDecode(e);
}

/*================================================================*/
bool KWDrag::decode( QMimeSource* e, QString& s )
{
    QByteArray ba = e->encodedData( MimeTypes[ 0 ] );
    if ( ba.size() )
    {
        s = QString::fromUtf8( ba.data(), ba.size() );
        return true;
    }
    return QTextDrag::decode(e, s);
}

