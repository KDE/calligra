/* This file is part of the KDE project
   Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

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
#include <qdom.h>

QDomElement KPImageKey::saveXML( QDomDocument &doc )
{
    QDomElement elem=doc.createElement( "BACKPIXKEY" );
    setAttributes( elem );
    return elem;
}

void KPImageKey::setAttributes( QDomElement &elem )
{
    QDate date = lastModified.date();
    QTime time = lastModified.time();
    elem.setAttribute( "filename", filename );
    elem.setAttribute( "year", date.year() );
    elem.setAttribute( "month", date.month() );
    elem.setAttribute( "day", date.day() );
    elem.setAttribute( "hour", time.hour() );
    elem.setAttribute( "minute", time.minute() );
    elem.setAttribute( "second", time.second() );
    elem.setAttribute( "msec", time.msec() );
}

void KPImageKey::loadAttributes( const QDomElement &elem, const QDate &dDate, const QTime &dTime )
{
    int year=dDate.year(), month=dDate.month(), day=dDate.day();
    int hour=dTime.hour(), minute=dTime.minute(), second=dTime.second(), msec=dTime.msec();
    if( elem.hasAttribute( "filename" ) )
        filename=elem.attribute( "filename" );
    if( elem.hasAttribute( "year" ) )
        year=elem.attribute( "year" ).toInt();
    if( elem.hasAttribute( "month" ) )
        month=elem.attribute( "month" ).toInt();
    if( elem.hasAttribute( "day" ) )
        day=elem.attribute( "day" ).toInt();
    if( elem.hasAttribute( "hour" ) )
        hour=elem.attribute( "hour" ).toInt();
    if( elem.hasAttribute( "minute" ) )
        minute=elem.attribute( "minute" ).toInt();
    if( elem.hasAttribute( "second" ) )
        second=elem.attribute( "second" ).toInt();
    if( elem.hasAttribute( "msec" ) )
        msec=elem.attribute( "msec" ).toInt();
    lastModified.setDate( QDate( year, month, day ) );
    lastModified.setTime( QTime( hour, minute, second, msec ) );
}

KoImage<KPImageKey> KPImageCollection::loadImage( const KPImageKey &key )
{
    KoImage<KPImageKey> res = findImage( key );

    if ( !res.isNull() )
        return res;

    QImage img( key.filename );
    if ( img.isNull() )
        return res;

    return insertImage( key, img );
}

KoImage<KPImageKey> KPImageCollection::loadImage( const KPImageKey &key, const QString &rawData )
{
    KoImage<KPImageKey> res = findImage( key );

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
