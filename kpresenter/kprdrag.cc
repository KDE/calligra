// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001,2004 Laurent Montel <montel@kde.org>

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

#include "kprdrag.h"
#include "kprdrag.moc"
#include <kdebug.h>
#include <kotextobject.h>

KPrTextDrag::KPrTextDrag( QWidget *dragSource, const char *name )
    : QTextDrag( dragSource, name )
{
}

QByteArray KPrTextDrag::encodedData( const char *mime ) const
{
    if ( ( strcmp( selectionMimeType(), mime ) == 0 ) || QString( mime ).startsWith( KoTextObject::acceptSelectionMimeType()) )
        return kpresenter;
    else
        return QTextDrag::encodedData(mime);
}

bool KPrTextDrag::provides( QMimeSource* e , const char* mimeType, const char* acceptMimeType, QString &returnedTypeMime)
{
    const char* fmt;
    for (int i=0; (fmt = e->format(i)); i++)
    {
        if ( !qstricmp(mimeType,fmt) || QString( fmt ).startsWith( acceptMimeType ))
        {
            returnedTypeMime = fmt;
            return TRUE;
        }
    }
    return FALSE;
}

bool KPrTextDrag::canDecode( QMimeSource* e )
{
    QString tmp;
    if ( KPrTextDrag::provides( e, selectionMimeType(), KoTextObject::acceptSelectionMimeType(), tmp ) )
        return true;
    return QTextDrag::canDecode(e);
}

const char* KPrTextDrag::format( int i ) const
{
    if ( i < 4 ) // HACK, but how to do otherwise ??
        return QTextDrag::format(i);
    else if ( i == 4 )
        return selectionMimeType();
    else if ( i == 5 )
        return "application/x-kpresenter-textobjectnumber";
    else return 0;
}

const char * KPrTextDrag::selectionMimeType()
{
    return "application/vnd.oasis.opendocument.presentation";
}
