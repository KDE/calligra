/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007-2008,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef TESTDOCUMENTFILESYNCHRONIZER_H
#define TESTDOCUMENTFILESYNCHRONIZER_H

// Kasten
#include <abstractmodelfilesystemsynchronizer.h>
// Qt
#include <QtCore/QByteArray>


namespace Kasten2
{

class TestDocument;


class TestDocumentFileSynchronizer : public AbstractModelFileSystemSynchronizer
{
    Q_OBJECT

  friend class TestDocumentFileLoadJob;
  friend class TestDocumentFileConnectJob;

  public:
    explicit TestDocumentFileSynchronizer( const QByteArray& header = QByteArray() );

  public: // AbstractModelSynchronizer API
    virtual AbstractLoadJob* startLoad( const KUrl& url );
    virtual AbstractSyncToRemoteJob* startSyncToRemote();
    virtual AbstractSyncFromRemoteJob* startSyncFromRemote();
    virtual AbstractSyncWithRemoteJob* startSyncWithRemote( const KUrl& url, AbstractModelSynchronizer::ConnectOption option );
    virtual AbstractConnectJob* startConnect( AbstractDocument* document,
                                              const KUrl& url, AbstractModelSynchronizer::ConnectOption option );

    virtual AbstractDocument* document() const;

    virtual LocalSyncState localSyncState() const;

  public:
    const QByteArray& header() const;

  protected:
    void setDocument( TestDocument* document );

  protected:
    TestDocument* mDocument;
    const QByteArray mHeader;
};


inline  const QByteArray& TestDocumentFileSynchronizer::header() const { return mHeader; }
inline void TestDocumentFileSynchronizer::setDocument( TestDocument* document ) { mDocument = document; }

}

#endif
