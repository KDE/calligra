/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>

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

#include <qdatetime.h>
#include <qdom.h>

#include "koPictureKey.h"


void KoPictureKey::saveAttributes( QDomElement &elem ) const
{
    QDate date = m_lastModified.date();
    QTime time = m_lastModified.time();
    elem.setAttribute( "filename", m_filename );
    elem.setAttribute( "year", date.year() );
    elem.setAttribute( "month", date.month() );
    elem.setAttribute( "day", date.day() );
    elem.setAttribute( "hour", time.hour() );
    elem.setAttribute( "minute", time.minute() );
    elem.setAttribute( "second", time.second() );
    elem.setAttribute( "msec", time.msec() );
}

void KoPictureKey::loadAttributes( const QDomElement &elem, const QDate &dDate, const QTime &dTime )
{
    int year=dDate.year(), month=dDate.month(), day=dDate.day();
    int hour=dTime.hour(), minute=dTime.minute(), second=dTime.second(), msec=dTime.msec();
    if( elem.hasAttribute( "key" ) )
        m_filename=elem.attribute( "key" ); // Old KWord format (up to 1.1-beta2)
    else
        m_filename=elem.attribute( "filename" );

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
    
    m_lastModified.setDate( QDate( year, month, day ) );
    m_lastModified.setTime( QTime( hour, minute, second, msec ) );
}

QString KoPictureKey::toString() const
{
    if ( m_lastModified.isValid() )
        // m_filename must be the last argument as it can contain a sequence starting with %
        return QString::fromLatin1("%2_%1").arg(m_lastModified.toString()).arg(m_filename);
    else
        return m_filename; // for kword-1.0's KWTextImage, which only has the filename in the key
}

