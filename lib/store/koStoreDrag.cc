/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <faure@kde.org>

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

#include "koStoreDrag.h"

QCString KoStoreDrag::mimeType( const char* nativeMimeType )
{
    return QCString(nativeMimeType); // + "-selection"; removed for OASIS
}

KoStoreDrag::KoStoreDrag( const char* nativeMimeType, QWidget *dragSource, const char *name )
    : QStoredDrag( mimeType(nativeMimeType), dragSource, name )
{
}

bool KoStoreDrag::canDecode( const char* nativeMimeType, QMimeSource* e )
{
    return e->provides( mimeType(nativeMimeType) );
}
