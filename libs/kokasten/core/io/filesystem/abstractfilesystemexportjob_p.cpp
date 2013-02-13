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

#include "abstractfilesystemexportjob_p.h"

// KDE
#include <KIO/NetAccess>
#include <KTemporaryFile>


namespace Kasten2
{

void AbstractFileSystemExportJobPrivate::exportToFile()
{
    Q_Q( AbstractFileSystemExportJob );

    bool isWorkFileOk;
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

    if( isWorkFileOk )
        q->startExportToFile();
    else
    {
        q->setError( KJob::KilledJobError );
        q->setErrorText( mFile->errorString() );

        q->completeExport( false );
    }
}

void AbstractFileSystemExportJobPrivate::completeExport( bool success )
{
    Q_Q( AbstractFileSystemExportJob );

    if( success )
    {
        if( ! mUrl.isLocalFile() )
        {
            success = KIO::NetAccess::upload( mWorkFilePath, mUrl, 0 );
            if( ! success )
            {
                q->setError( KJob::KilledJobError );
                q->setErrorText( KIO::NetAccess::lastErrorString() );
            }
        }
    }
    else
    {
        q->setError( KJob::KilledJobError );
        q->setErrorText( mFile->errorString() );
    }

    delete mFile;

    q->emitResult();
}

}
