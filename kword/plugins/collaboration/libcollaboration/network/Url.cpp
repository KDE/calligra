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
#include "Url.h"
#include <QStringList>
using namespace kcollaborate;

Url::Url( const QString &url ):
        KUrl( url ), parsed( false ), hostConverted( false )
{}

Url::~Url()
{}

const QString & Url::subprotocol()
{
    if ( !parsed ) parse();
    return subprotocol_;
}

const QString & Url::invitationNumber()
{
    if ( !parsed ) parse();
    return invitationNumber_;
}

const QString & Url::documentName()
{
    if ( !parsed ) parse();
    return documentName_;
}

void Url::parse()
{
    QString path = KUrl::path();
    QStringList parts = path.split( '/', QString::SkipEmptyParts );
    if ( "collaborate" != protocol() || parts.size() != 3 ) { //broken url
        broken_ = true;
        subprotocol_ = "";
        invitationNumber_ = "";
        documentName_ = "";
    } else {
        broken_ = false;
        subprotocol_ = parts.at( 0 );
        invitationNumber_ = parts.at( 1 );
        documentName_ = parts.at( 2 );
    }
    parsed = true;
}

void Url::setPath( const QString &path )
{
    KUrl::setPath( path );
    parsed = false;
    hostConverted = false;
}

bool Url::broken()
{
    if ( !parsed ) parse();
    if ( !broken_ && !hostConverted ) hostConvert();
    return broken_;
}

void Url::hostConvert()
{
    if ( hostConverted ) return;
    hostAddress_.setAddress( host() );
    hostConverted = true;
}

const QHostAddress& Url::hostAddress()
{
    if ( !hostConverted ) hostConvert();
    return hostAddress_;
}

#include "Url.moc"
