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

#ifndef ABSTRACTFILESYSTEMCONNECTJOB_P_H
#define ABSTRACTFILESYSTEMCONNECTJOB_P_H

// library
#include "abstractfilesystemconnectjob.h"
#include <abstractconnectjob_p.h>
// KDE
#include <KUrl>
#include <KTemporaryFile>


namespace Kasten2
{

class KASTENCORE_EXPORT AbstractFileSystemConnectJobPrivate : public AbstractConnectJobPrivate
{
  public:
    AbstractFileSystemConnectJobPrivate( AbstractFileSystemConnectJob* parent,
                                         AbstractModelFileSystemSynchronizer* synchronizer, AbstractDocument* document,
                                         const KUrl& url, AbstractModelSynchronizer::ConnectOption option );

    virtual ~AbstractFileSystemConnectJobPrivate();

  public: // KJob API
    void start();

  public:
    void complete( bool success );

  public:
    AbstractModelFileSystemSynchronizer* synchronizer() const;
    AbstractDocument* document() const;
    QFile* file() const;

  public:
    void connectWithFile();

  protected:
    Q_DECLARE_PUBLIC( AbstractFileSystemConnectJob )

  protected:
    AbstractModelFileSystemSynchronizer* const mSynchronizer;
    AbstractDocument* const mDocument;
    const KUrl mUrl;
    const AbstractModelSynchronizer::ConnectOption mOption;
    QFile* mFile;
    QString mWorkFilePath;
};


inline AbstractFileSystemConnectJobPrivate::AbstractFileSystemConnectJobPrivate( AbstractFileSystemConnectJob* parent,
    AbstractModelFileSystemSynchronizer* synchronizer, AbstractDocument* document,
    const KUrl& url, AbstractModelSynchronizer::ConnectOption option )
  : AbstractConnectJobPrivate( parent ),
    mSynchronizer( synchronizer ),
    mDocument( document ),
    mUrl( url ),
    mOption( option ),
    mFile( 0 )
{}

inline AbstractFileSystemConnectJobPrivate::~AbstractFileSystemConnectJobPrivate() {}

inline AbstractModelFileSystemSynchronizer* AbstractFileSystemConnectJobPrivate::synchronizer() const
{
    return mSynchronizer;
}
inline AbstractDocument* AbstractFileSystemConnectJobPrivate::document()   const { return mDocument; }
inline QFile* AbstractFileSystemConnectJobPrivate::file()                  const { return mFile; }
// TODO: setup a notification system

inline void AbstractFileSystemConnectJobPrivate::start()
{
    Q_Q( AbstractFileSystemConnectJob );

    QMetaObject::invokeMethod( q, "connectWithFile", Qt::QueuedConnection );
}

}

#endif
