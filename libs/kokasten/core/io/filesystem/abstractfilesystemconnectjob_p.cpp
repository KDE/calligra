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

#include "abstractfilesystemconnectjob_p.h"

// library
#include "abstractmodelfilesystemsynchronizer.h"
#include <abstractdocument.h>
// KDE
#include <KIO/NetAccess>
// Qt
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>


namespace Kasten2
{

void AbstractFileSystemConnectJobPrivate::connectWithFile()
{
    Q_Q( AbstractFileSystemConnectJob );

// Comment: here we play tricks to reuse the temporary file
// KIO::NetAccess::removeTempFile only removes tempfiles created by KIO::NetAccess::download
// So if replaceRemote and workFilePath is temporaryFile both don't conflict -> no problem (now)

    bool isWorkFileOk;
    if( mOption == AbstractModelSynchronizer::ReplaceRemote )
    {
        if( mUrl.isLocalFile() )
        {
            mWorkFilePath = mUrl.path();
            mFile = new QFile( mWorkFilePath );
            isWorkFileOk = mFile->open( QIODevice::WriteOnly );
        }
        else
        {
            KTemporaryFile* temporaryFile = new KTemporaryFile;
            isWorkFileOk = temporaryFile->open();

            mWorkFilePath = temporaryFile->fileName();
            mFile = temporaryFile;
        }
    }
    else
    {
        isWorkFileOk = KIO::NetAccess::download( mUrl.url(), mWorkFilePath, 0 );
        if( isWorkFileOk )
        {
            mFile = new QFile( mWorkFilePath );
            isWorkFileOk = mFile->open( QIODevice::ReadOnly );
        }
    }

    if( isWorkFileOk )
        q->startConnectWithFile();
    else
    {
        q->setError( KJob::KilledJobError );
        q->setErrorText( mFile ? mFile->errorString() : KIO::NetAccess::lastErrorString() );
        delete mFile;
        // TODO: should we rather skip setDocument in the API?
        q->emitResult();
    }
}

void AbstractFileSystemConnectJobPrivate::complete( bool success )
{
    Q_Q( AbstractFileSystemConnectJob );

    if( success )
    {
        mFile->close(); // TODO: when is new time written, on close?
        QFileInfo fileInfo( *mFile );
        mSynchronizer->setFileDateTimeOnSync( fileInfo.lastModified() );

        mSynchronizer->setUrl( mUrl );

        if( ! mUrl.isLocalFile() )
        {
            const bool uploaded = KIO::NetAccess::upload( mWorkFilePath, mUrl, 0 );
            if( ! uploaded )
            {
                q->setError( KJob::KilledJobError );
                q->setErrorText( KIO::NetAccess::lastErrorString() );
            }
            else
            {
                mSynchronizer->startNetworkWatching();
                mSynchronizer->setRemoteState( RemoteUnknown );
            }
        }
        else
        {
            mSynchronizer->startFileWatching();
            mSynchronizer->setRemoteState( RemoteInSync );
        }

        // TODO; in path of both constructor by url and synchWithRemote
        // only needed for the first, so constructor writers can forget about this
        // for now we just check in setSynchronizer that new != old before deleting old
        mDocument->setSynchronizer( mSynchronizer );
    }
    else
    {
        delete mSynchronizer;
        // TODO: these reports should go to a notification system, for log or popup
        q->setError( KJob::KilledJobError );
        q->setErrorText( mFile->errorString() );
    }

    delete mFile;
    KIO::NetAccess::removeTempFile( mWorkFilePath );

    q->emitResult();
}

}
