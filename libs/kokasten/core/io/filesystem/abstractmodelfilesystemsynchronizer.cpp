/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "abstractmodelfilesystemsynchronizer.h"
#include "abstractmodelfilesystemsynchronizer_p.h"
#include "abstractmodelfilesystemsynchronizer.moc"

// lib
#include <abstractdocument.h>


namespace Kasten2
{

AbstractModelFileSystemSynchronizer::AbstractModelFileSystemSynchronizer( AbstractModelFileSystemSynchronizerPrivate* d )
  : AbstractModelSynchronizer( d )
{
}

AbstractModelFileSystemSynchronizer::AbstractModelFileSystemSynchronizer()
  : AbstractModelSynchronizer( new AbstractModelFileSystemSynchronizerPrivate(this) )
{}

RemoteSyncState AbstractModelFileSystemSynchronizer::remoteSyncState() const
{
    Q_D( const AbstractModelFileSystemSynchronizer );

    return d->remoteSyncState();
}

void AbstractModelFileSystemSynchronizer::setRemoteState( RemoteSyncState remoteState )
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->setRemoteState( remoteState );
}

void AbstractModelFileSystemSynchronizer::setFileDateTimeOnSync( const QDateTime& fileDateTime )
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->setFileDateTimeOnSync( fileDateTime );
}

void AbstractModelFileSystemSynchronizer::startFileWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->startFileWatching();
}

void AbstractModelFileSystemSynchronizer::stopFileWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->stopFileWatching();
}

void AbstractModelFileSystemSynchronizer::pauseFileWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->pauseFileWatching();
}

void AbstractModelFileSystemSynchronizer::unpauseFileWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->unpauseFileWatching();
}

void AbstractModelFileSystemSynchronizer::startNetworkWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->startNetworkWatching();
}

void AbstractModelFileSystemSynchronizer::stopNetworkWatching()
{
    Q_D( AbstractModelFileSystemSynchronizer );

    d->stopNetworkWatching();
}

AbstractModelFileSystemSynchronizer::~AbstractModelFileSystemSynchronizer()
{
}

}
