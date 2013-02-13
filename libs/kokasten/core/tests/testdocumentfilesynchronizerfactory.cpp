/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "testdocumentfilesynchronizerfactory.h"

// lib
#include "testdocument.h"
#include "testdocumentfilesynchronizer.h"
// Qt
#include <QtCore/QLatin1String>


namespace Kasten2
{

TestDocumentFileSynchronizerFactory::TestDocumentFileSynchronizerFactory( const QByteArray& header )
 : mHeader( header ) {}

// could be set to base class as value, is only one object per factory at runtime
// and settable in desktop file
QString TestDocumentFileSynchronizerFactory::supportedWorkType() const { return QLatin1String("TestDocument");}
QString TestDocumentFileSynchronizerFactory::supportedRemoteType() const { return QLatin1String("application/octet-stream");}

// TODO: these function seems to be always the same. Make macro or template function
// or, if there is only one place which calls this, move there
AbstractModelSynchronizer* TestDocumentFileSynchronizerFactory::createSynchronizer() const
{
    return new TestDocumentFileSynchronizer( mHeader );
}

TestDocumentFileSynchronizerFactory::~TestDocumentFileSynchronizerFactory() {}

}
