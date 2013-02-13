/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "abstractfilesystemsyncfromremotejob_p.h"

// library
#include "abstractmodelfilesystemsynchronizer.h"
// KDE
#include <KIO/NetAccess>
// Qt
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>


namespace Kasten2
{

void AbstractFileSystemSyncFromRemoteJobPrivate::syncFromRemote()
{
    Q_Q( AbstractFileSystemSyncFromRemoteJob );

    const KUrl url = mSynchronizer->url();

    // TODO: see if this could be used asynchronously instead
    bool isWorkFileOk = KIO::NetAccess::download( url.url(), mWorkFilePath, 0 );
    if( isWorkFileOk )
    {
        mFile = new QFile( mWorkFilePath );
        isWorkFileOk = mFile->open( QIODevice::ReadOnly );
    }

    if( isWorkFileOk )
        q->startReadFromFile();
    else
    {
        q->setError( KJob::KilledJobError );
        q->setErrorText( mFile ? mFile->errorString() : KIO::NetAccess::lastErrorString() );
        delete mFile;
        q->emitResult();
    }
}

void AbstractFileSystemSyncFromRemoteJobPrivate::completeRead( bool success )
{
    Q_Q( AbstractFileSystemSyncFromRemoteJob );

    if( success )
    {
        const KUrl url = mSynchronizer->url();
        const bool isLocalFile = url.isLocalFile();

        QFileInfo fileInfo( mWorkFilePath );
        mSynchronizer->setFileDateTimeOnSync( fileInfo.lastModified() );
        mSynchronizer->setRemoteState( isLocalFile ? RemoteInSync : RemoteUnknown );
    }

    delete mFile;
    KIO::NetAccess::removeTempFile( mWorkFilePath );

    q->emitResult();
}

}
