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

#include <kpclipartcollection.h>
#include <qwmf.h>
#include <qdom.h>

KPClipart KPClipartCollection::findOrLoad( const QString & fileName, const QDateTime & dateTime )
{
    ASSERT( !fileName.isEmpty() );
    if ( dateTime.isValid() )
        return findClipart( KPClipartKey( fileName, dateTime ) );
    else
        return loadClipart( fileName );
}

#if 0
void KPClipartCollection::addRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

//     qDebug( "KPClipartCollection::addRef" );

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = ++ref;
//      qDebug( "    ref: %d", ref );
    }
}

/*================================================================*/
void KPClipartCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

//     qDebug( "KPClipartCollection::removeRef" );

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = --ref;
//      qDebug( "     ref: %d", ref );
        if ( ref == 0 ) {
//          qDebug( "        remove" );
            refs.remove( key );
            data.remove( key );
        }
    }
}
#endif
