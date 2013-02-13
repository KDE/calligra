/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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


#include "testdocumentfilereloadjob.h"

// lib
#include "testdocumentfilesynchronizer.h"
#include "testdocumentfilereloadthread.h"
#include "testdocument.h"
// Qt
#include <QtGui/QApplication>


namespace Kasten2
{

TestDocumentFileReloadJob::TestDocumentFileReloadJob( TestDocumentFileSynchronizer* synchronizer )
 : AbstractFileSystemSyncFromRemoteJob( synchronizer )
{}

void TestDocumentFileReloadJob::startReadFromFile()
{
    TestDocumentFileSynchronizer* testSynchronizer = qobject_cast<TestDocumentFileSynchronizer*>( synchronizer() );
    TestDocument* document = qobject_cast<TestDocument*>( synchronizer()->document() );
    TestDocumentFileReloadThread* reloadThread =
        new TestDocumentFileReloadThread( this, testSynchronizer->header(), /*document, */file() );
    reloadThread->start();
    while( !reloadThread->wait(100) )
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 100 );

    const bool success = reloadThread->success();
    // TODO: moved this here to avoid marshalling the change signals out of the thread. Good idea?
    if( success )
    {
        document->setData( reloadThread->byteArray() );
    }
    delete reloadThread;

    completeRead( success );
}

TestDocumentFileReloadJob::~TestDocumentFileReloadJob()
{
}

}
