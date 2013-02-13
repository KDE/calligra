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

#include "testdocumentfileloadthread.h"

// lib
#include "testdocument.h"
// KDE
#include <KUrl>
#include <KLocale>
// Qt
#include <QtCore/QByteArray>
#include <QtGui/QApplication>
#include <QtCore/QDataStream>
#include <QtCore/QFile>


namespace Kasten2
{

void TestDocumentFileLoadThread::run()
{
    QDataStream inStream( mFile );
    const int fileSize = mFile->size();

    // test header
    const int headerSize = mHeader.size();
    QByteArray header( headerSize, ' ' );
    const int headerResult = inStream.readRawData( header.data(), headerSize );
    if( headerResult == -1 || header != mHeader )
        mDocument = 0;
    else
    {
        QByteArray byteArray( fileSize, ' ' );

        inStream.readRawData( byteArray.data(), fileSize );

        //registerDiskModifyTime( file ); TODO move into synchronizer

        const bool streamIsOk = ( inStream.status() == QDataStream::Ok );
    //     if( success )
    //         *success = streamIsOk ? 0 : 1;
        if( streamIsOk )
        {
            mDocument = new TestDocument( byteArray );
            mDocument->moveToThread( QApplication::instance()->thread() );
        }
        else
        {
            mDocument = 0;
        }
    }

    emit documentRead( mDocument );
}

TestDocumentFileLoadThread::~TestDocumentFileLoadThread() {}

}
