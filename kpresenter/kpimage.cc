/* This file is part of the KDE project
   Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

#include "kpimage.h"

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kpimage.h>

KPImage KPImageCollection::findOrLoad( const QString & fileName, const QDateTime & dateTime )
{
    ASSERT( !fileName.isEmpty() );
    if ( dateTime.isValid() )
        return findImage( KoImageKey( fileName, dateTime ) );
    else
        return loadImage( fileName );
}

KPImage KPImageCollection::loadXPMImage( const KPImageKey &key, const QString &rawData )
{
    KPImage res = findImage( key );

    if ( !res.isNull() )
        return res;

    QCString s(rawData.latin1());
    int i = s.find( ( char )1, 0 );

    while ( i != -1 ) {
        s[ i ] = '\"';
        i = s.find( ( char )1, i + 1 );
    }

    QImage img;
    img.loadFromData( s, "XPM" );

    if ( img.isNull() )
        return res;

    return insertImage( key, img );
}
