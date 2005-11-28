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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrTextDrag.h"
#include "KPrTextDrag.moc"
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

bool KPrTextDrag::canDecode( QMimeSource* e )
{
    if ( KoTextObject::providesOasis( e ) )
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
    //return "application/vnd.oasis.opendocument.presentation";
    return "application/x-kpresenter-textselection";
}
