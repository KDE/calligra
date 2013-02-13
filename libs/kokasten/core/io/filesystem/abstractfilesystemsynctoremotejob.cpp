/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "abstractfilesystemsynctoremotejob.h"
#include "abstractfilesystemsynctoremotejob_p.h"
#include "abstractfilesystemsynctoremotejob.moc"


namespace Kasten2
{

AbstractFileSystemSyncToRemoteJob::AbstractFileSystemSyncToRemoteJob( AbstractModelFileSystemSynchronizer* synchronizer )
  : AbstractSyncToRemoteJob( new AbstractFileSystemSyncToRemoteJobPrivate(this,synchronizer) )
{
}

AbstractModelFileSystemSynchronizer* AbstractFileSystemSyncToRemoteJob::synchronizer() const
{
    Q_D( const AbstractFileSystemSyncToRemoteJob );

    return d->synchronizer();
}

QFile* AbstractFileSystemSyncToRemoteJob::file() const
{
    Q_D( const AbstractFileSystemSyncToRemoteJob );

    return d->file();
}

void AbstractFileSystemSyncToRemoteJob::start()
{
    Q_D( AbstractFileSystemSyncToRemoteJob );

    d->start();
}

void AbstractFileSystemSyncToRemoteJob::completeWrite( bool success )
{
    Q_D( AbstractFileSystemSyncToRemoteJob );

    d->completeWrite( success );
}


AbstractFileSystemSyncToRemoteJob::~AbstractFileSystemSyncToRemoteJob()
{
}

}
