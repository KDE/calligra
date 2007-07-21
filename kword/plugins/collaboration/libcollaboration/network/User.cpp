/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "User.h"
using namespace kcollaborate;

User::User( const QString &name, const QString &pictureUrl, QObject *parent ):
        QObject( parent ), m_name( name ), m_pictureUrl( pictureUrl )
{}

User::~User()
{}

const QString& User::name() const
{
    return m_name;
}

const QString& User::pictureUrl() const
{
    return m_pictureUrl;
}

const QMap<QString, QString>& User::im() const
{
    return m_im;
}

const QString User::im( const QString &imType ) const
{
    return m_im[imType];
}

void User::addIm( const QString &imType, const QString &account )
{
    m_im[imType] = account;
}

const QString User::toMsgPart() const
{
    QString out;
    out.reserve( 1024 );
    out.append( "<User>\n" );
    out.append( "<Name>" ).append( name() ).append( "</Name>\n" );
    out.append( "<PictureUrl>" ).append( pictureUrl() ).append( "</PictureUrl>\n" );
    if ( !im().empty() ) {
        out.append( "<Im>\n" );

        QMapIterator<QString, QString> i( im() );
        while ( i.hasNext() ) {
            i.next();
            out.append( "<" ).append( i.key() ).append( ">" );
            out.append( i.value() );
            out.append( "</" ).append( i.key() ).append( ">\n" );
        }

        out.append( "</Im>\n" );
    }
    out.append( "</User>" );
    return out;
}

#include "User.moc"
